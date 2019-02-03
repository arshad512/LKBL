#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h> /* support kernel driver model */
#include <linux/kernel.h>
#include <linux/errno.h>  /* error codes */
#include <linux/sched.h>
#include <linux/mm.h> /* find_vma */
#include <linux/mm_types.h> /* vm_area_struct */
#include <linux/slab.h>
#include <linux/fs.h> /* file system specifics */
#include <linux/uaccess.h> /* Require for copy to user function */
#include <linux/vmalloc.h>
#include <linux/highmem.h> /* kmap/kunmap */
#include <linux/types.h> /* dev_t * defs */
#include <linux/bio.h> /* BIO */
//#include <linux/blk_types.h> /* struct bio, bio_vec and BIO_* flags */
#include <linux/blkdev.h>
#include <linux/list.h>
#include <linux/version.h> /* KERNEL_VERSION */


/* SCSI */
#include <scsi/scsi_device.h>
#include <scsi/scsi_host.h>
#include <scsi/scsi_devinfo.h>
#include <scsi/sg.h>

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Basic Block driver");

#define SIZE_IN_KBYTES 256

static int major;
static struct gendisk *disk;
static struct request_queue *bdqueue;
static char *mempool;

DEFINE_SPINLOCK(bdlock);

/*
 *
 * =======================
 * Block device operation.
 * =======================
 *
 * 01. open: Is called directly from user space. eg, creating partition,
 * 	FS creation, fsck. In mount() operation open is called directly
 * 	from kernel space. File descriptor, is stored in kernel. So open()
 * 	can not find out if open is from kernel space or user space.
 * 02. release: Opposite of open.
 * 03. ioctl
 * 04. direct_access
 * 05. media_changed
 * 06. revalidate_disk
 * 07. getgeo
 *
 * No, read/write as that is done through request() function.
 */
static struct block_device_operations bops = {
	.owner = THIS_MODULE,
};

static void foo1(void)
{
	printk("Inside foo1\n");
}
EXPORT_SYMBOL(foo1);

/*
 * Function associated with block queue is of type "request_fn_proc"
 * Block device do not have read/write as character device driver have it.
 * This function acts an equivalent to read/write of char driver.
 *
 * If its called from within "request_fn_proc" type, the spin-lock 
 * is already held. Else calling blk_fetch_request() must have a
 * spin-lock
 *
 */
static void foo(struct request_queue *q)
{
	/*
	 * Each 'struct request' is an IO block request.
	 * This can be an combination of one or more request 
	 * from higher layer. Each request is a buffer of 
	 * memory, that needs to be read from device or written
	 * to device.
	 *
	 * Remember, sectors to be transfer can be scattered
	 * in main memory but are consecutive on device.
	 *
	 * A single request can have a mix of many sectors but
	 * _CANNOT_ have read/write mixed in single request.
	 *
	 * Single request queue can be shared across multiple block 
	 * device. However, normal use is to create separate request 
	 * queue for each physical/block drive.
	 *
	 * Request are sorted into an order in request queue for
	 * performance benefits. Drivers can still re-order the
	 * request in case of 'tagged command queueing' (TCQ) and 
	 * write cacheing, to complete in order. To stop the reordering
	 * eg: Journal FS which force journal entries to disk before
	 * the operations they describe.
	 *
	 * Barrier-request are marked by 'REQ_HARDBARRIER' flag, 
	 * in 'struct request' flag field. This means, It _must_
	 * complete this request + All request before this before beginning
	 * any other new request.
	 *
	 * The default is the usual 512-byte sector. There is one other
	 * important change with regard to sector sizes: your driver will
	 * always see requests expressed in terms of 512-byte sectors,
	 * regardless of the hardware sector size. The block subsystem
	 * will not generate requests which go against the hardware
	 * sector size, but sector numbers and counts in requests
	 * are always in 512-byte units. This change was required
	 * as part of the new centralized partition remapping.
	 *
	 * A low level description of block request(struct request)
	 * is 'struct bio'. A request contains a list of BIO
	 * structures.
	 *
	 * struct bio contains:
	 * 	struct bio_vec {
	 * 		struct page; (Pages in physical memory)
	 * 		offset
	 * 		size of buffer
	 * 	}
	 *
	 * Therefore to iterate over struct bio we must 
	 * iterate over 'bio_vec' and transfer data from every
	 * physical page, using struct 'bi_iter'.
	 *
	 * Bio is normally created by FS layer and then
	 * transmitted to the IO subsystem. This is done
	 * through submit_bio() call. Which adds bio to the
	 * request queue of the device. Bio, will then be 
	 * processed by the "request_fn_proc" function.
	 *
	 * To notify processing of 'struct bio' ends the
	 * bi_end_io should be used. This field specify function to 
	 * be called at the end of bio processing.
	 *
	 * bi_private field can be used to pass information to the function.
	 */
	struct request *req;
	struct req_iterator iter;
	struct bio_vec bvec;
	int r_count=0;
	int w_count=0;

	while (1) {
		/*
		 * blk_fetch_request():
		 * Retrieve the 'FIRST' request from queue
		 * and starts it. 
		 * REMEMBER!!! - It starts queue processing
		 */
		req = blk_fetch_request(q);
		if (!req)
			break;

		/* 
		 * A block device can get calls which do 
		 * not involve transfer of data. Like low level
		 * operations on Disk. (LUN RESET etc..).
		 * 
		 * In this case. Close the request queue,
		 * even if there is pending request and return EIO.
		 * Here, we can also ignore non disk transfer requests.
		 * 	_blk_end_request_all(req,0);
		 * 	continue;
		 */
		if (blk_rq_is_passthrough(req)) {
			printk(KERN_INFO "Check if it is a pass through...\n");
			continue;
		}
		
		rq_for_each_segment(bvec, req, iter) {
			if(bio_data_dir(req->bio) == READ) {
				/*
				 * Read request
				 */
				r_count++;
			} else {
				/*
				 * write request:
				 *
				 * Read sector. Which may be empty.
				 * Update with latest buffer to write
				 * finally write it.
				 *
				 */
				w_count++;
			}
		}
		
		/* 
		 * Check if the current request is done or still
		 * data pending.
		 *
		 * When Driver has finished transferring all the sectors
		 * of the request to and from the device. It must
		 * inform the IO subsystem. And that is done through
		 * a function call blk_end_request() if lock not already
		 * taken and blk_end_request() if lock is already taken.
		 * then call __blk_end_request_cur()
		 */
		if(!__blk_end_request_cur(req,0)) {
			// Yes there is still data left.
			continue;
		}
	}
	printk(KERN_INFO "Total read is %d\n", r_count);
	printk(KERN_INFO "Total writ is %d\n", w_count);
}

static int __init lbl_init(void) 
{
	/*
	 * Register a block IO device.
	 * Provides a major number for block device. eg /dev/bds
	 * Allocate major dynamically. 0 passed as first arg
	 * indicate the major allocation is dynamic.
	 */
	if ((major=register_blkdev(0,"bds")) == 0) {
		printk("registering failed\n");
		return -EIO;
	}

	if (!(mempool = vmalloc(SIZE_IN_KBYTES * 1024))) {
		printk("vmalloc failed...\n");
		goto out_no_mem;
	}

	/*
	 * Allocate a disk.
	 * This means, allocate space for struct 'gendisk' which will
	 * represent the actual disk.
	 * Gendisk, stores information about the actual disk.
	 */
	if (!(disk = alloc_disk(1))) {
		printk("alloc_disk failed...\n");
		goto out;
	}

	/*
	 * Fill in the gendisk struct
	 */

	/* provide major :- got from register_blkdev */
	disk->major = major;
	/* provide minor - partition information*/
	disk->first_minor = 0;
	/* assign block operation */
	disk->fops = &bops;

	/*
	 * A queue is associated with a device driver.
	 * Device queue stores IO in flight that would be process
	 * by the Block layer.
	 *
	 * Queue is of type "struct request_queue".
	 * Request are added to this queue by higher layer (eg FS).
	 * It is the job of block layer to pick and process IO
	 * as long as the queue is not empty.
	 * Each item in request_queue is of type "struct request"
	 *
	 * Remember: Request queue can be tuned with any one of the
	 * IO schedulers. A scheduler sorts & merges the request in
	 * a manner that maximize performance.
	 *
	 * =========================================================
	 * "make request" or Doing without request queue (side note)
	 * =========================================================
	 * Some device have no real need for request queue (sort/merge)
	 * example: ramdisk,MDT. They do not benefit from request queue.
	 * Much better the driver handle the request themself. For this
	 * the driver uses "make request" function.
	 * For this the request queue is still created, but not used to 
	 * house actual request but other information of block subsystem.
	 * 01. Create queue with blk_alloc_queue() - Function pointer of
	 * 	type request_queue_t(requtest_queue_t *q, stuct bio *bio);
	 * 02. if the "request_queue_t/make_request" can handle bio
	 * 	it should return 0, and pass BIO to bio_endio().
	 * 	Else change "bi_bdev" and return non-zero.
	 * 	The bio will be dispatched to new device driver.
	 */

	/*
	 * Request Queue is created using blk_init_queue()
	 * 1st argument: Function pointer which processes the request for
	 * the device
	 * 2nd argument: Is the spin-lock. Lock when processing the 
	 * request.
	 *
	 * blk_init_queue() is the block subsystem call.
	 *
	 */
	if ((bdqueue = blk_init_queue(foo, &bdlock)) == NULL) {
		printk(" Init queue falied\n");
		goto out;
	}

	/*
	 * Disk device sector size. All operations hence forth
	 * on this device would be multiple of values set.
	 * In this case 512 bytes.
	 */
	blk_queue_logical_block_size(bdqueue, 512);
	/* Associate newly created queue with device */
	disk->queue = bdqueue;
	sprintf(disk->disk_name, "bds0");
	/* Set disk capacity */
	set_capacity(disk, (SIZE_IN_KBYTES * 1024) >> 9);
	/*
	 * Make disk alive
	 */
	add_disk(disk);
	printk("module loaded...\n");
	foo1();
	return 0;
out:
	vfree(mempool);
out_no_mem:
	unregister_blkdev(major, "bds");
	return -EIO;
}

static void __exit lbl_exit(void) {
	printk(KERN_INFO "Goodbye!\n");
	/*
	 * Un-register a block io device
	 */
	unregister_blkdev(major, "bds");
	/* remove gendisk structure */
	del_gendisk(disk);
	/* make disk dead : cannot receive any structure */
	put_disk(disk);
	/* cleanup queue */
	blk_cleanup_queue(bdqueue);
	vfree(mempool);
}

module_init(lbl_init);
module_exit(lbl_exit);

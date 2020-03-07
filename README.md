# LKBL

## Journaling

System crash can leave FS in inconsistent state. This can be repaired using FS checkers, however they are slow. Modern FS prefer journaling also known as “Write-ahead” logging. It is a technique when before actual writing of data, operation is logged. This helps in case of crash where only the journal log is replayed instead of whole disk scan. Journal can be a file within FS or different device.  Journal normally contains a “Transaction” start, followed by what needs to be updated, finally it has an end marker called “transaction” end. These set of information have enough information to replay in case of crash. A Journaled FS, first writes the journaled log (also only called log) and once this is successful it “checkpoints” or updates the metadata with the latest information.

Every write can be divide into two operations. 1) First, write to metadata, 2) Second, The Data itself. (block write)

#### There are two types of journal setup:-
1.	Data journaling - Where both metadata and data are journaled. Downside, is that data is written twice.
2.	Metadata journaling (ordered) - Where only metadata is journaled. In this case data is first wirtten followed by Metadata.

#### Cases where FS can in inconsistent state
### Metadata Failed and Data failed
-	FS is consistent. All failure.

### Metadata failed but Data passed.
-	Here there will no updated inode pointing to data.
-	from FS point of view it is still consistent 

### Metadata passed but Data failed
-	Inode points to latest updated data. Which is not there but garbage 
-	FS is in in-consistent state. 

Write barrieres gurantees that the write before the issue of barrier would be executed first and would not be reorderd due to buffereing or cacheing.
Journaling

## Linux Kernel Block Layer

1. blk_queue_flag_set
1. blk_queue_flag_clear
1. blk_queue_flag_test_and_set
1. blk_rq_init
1. errno_to_blk_status
1. blk_status_to_errno
1. blk_dump_rq_flags
1. blk_sync_queue
1. blk_set_pm_only
1. blk_clear_pm_only
1. blk_put_queue
1. blk_set_queue_dying
1. blk_cleanup_queue
1. blk_alloc_queue
1. blk_alloc_queue_node
1. blk_get_queue
1. blk_get_request
1. blk_put_request
1. blk_init_request_from_bio
1. generic_make_request
1. direct_make_request
1. submit_bio
1. blk_insert_cloned_request
1. blk_rq_err_bytes
1. blk_steal_bios
1. blk_update_request
1. rq_flush_dcache_pages
1. blk_lld_busy
1. blk_rq_unprep_clone
1. blk_rq_prep_clone
1. kblockd_schedule_work
1. kblockd_schedule_work_on
1. kblockd_mod_delayed_work_on
1. blk_start_plug
1. blk_check_plugged
1. blk_finish_plug
1. blkdev_issue_discard
1. blkdev_issue_write_same
1. blkdev_issue_zeroout
1. fs_bio_set
1. bio_uninit
1. bio_init
1. bio_reset
1. bio_chain
1. bio_alloc_bioset
1. zero_fill_bio_iter
1. bio_put
1. bio_clone_fast
1. bio_add_pc_page
1. bio_add_page
1. submit_bio_wait
1. bio_advance
1. bio_copy_data_iter
1. bio_copy_data
1. bio_list_copy_data
1. bio_free_pages
1. bio_map_kern
1. generic_start_io_acct
1. generic_end_io_acct
1. bio_flush_dcache_pages
1. bio_endio
1. bio_split
1. bio_trim
1. bioset_exit
1. bioset_init
1. bioset_init_from_src
1. bio_disassociate_blkg
1. bio_associate_blkg_from_css
1. bio_associate_blkg
1. bio_clone_blkg_association

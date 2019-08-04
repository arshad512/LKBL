# SCSI (TODO)

1. Initiator

These are clients wich initiates the request. This first might have to go through authentation is it is setup.
1. Target

Targets are servers. They execute the request and respond to client.
1. Task

A SCSI command to execute is also known as a Task.
1. Nexus

Also known as a session. Is connection between initiator port to target port
1. LUN

Logical Unit. A slice of Disk :)

1. SCSI Command:
1. CDB

Is a structre with contains the SCSI Command to be executed by the Target.
2. PDU

1. ITT

Initiator Task Tag is related to a task and is valid for the life of the tag.

1. Status

Contains SCSI status after execution in target

1. Sense Data

If SCSI command fails it will contain sense data.



3. cmdSN

Sequence number send by Initiator to Target. Initiator diliver tasks in cmdSN order.

1. statSN

Sequence number send by Target to Initiator. expectedStatSN is then maintained by initiator to detect any missing PDU.

1. Unit attention 
1. SCSI Primary Commands (SPC)
1. SCSI Block Devices
1. Mapping
2. R2T

Ready to transfer. When initiator wants to write. Target sends a R2T, for with initiator sends DATA.

2. TTT

Target transfer tag. Send by target to initiator for each data transfer.

1. Storage Array controllers
1. SAC Commands
1. Mapping

1. Fibre Channel

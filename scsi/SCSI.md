# SCSI/iSCSI (TODO)

#### Initiator
These are clients wich initiates the request. This first might have to go through authentation is it is setup.
#### Target
Targets are servers. They execute the request and respond to client.
#### Task
A SCSI command to execute is also known as a Task.
#### Nexus
Also known as a session. Is connection between initiator port to target port
#### LUN
Logical Unit. A slice of Disk :)
#### SCSI Command:
TODO
#### CDB
Is a structre with contains the SCSI Command to be executed by the Target.
#### PDU
Contains CDB
#### ITT
Initiator Task Tag is related to a task and is valid for the life of the tag.
#### Status
Contains SCSI status after execution in target
#### Sense Data
If SCSI command fails it will contain sense data.
#### cmdSN
Sequence number send by Initiator to Target. Initiator diliver tasks in cmdSN order.
#### statSN
Sequence number send by Target to Initiator. expectedStatSN is then maintained by initiator to detect any missing PDU.
#### Unit attention 
TODO
#### SCSI Primary Commands (SPC)
TODO
#### SCSI Block Devices
TODO
#### Mapping
#### R2T
Ready to transfer. When initiator wants to write. Target sends a R2T, for with initiator sends DATA.
#### TTT
Target transfer tag. Send by target to initiator for each data transfer.
#### Storage Array controllers
TODO
#### SAC Commands
TODO
#### Mapping
#### Fibre Channel

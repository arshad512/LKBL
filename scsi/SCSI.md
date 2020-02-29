# SCSI/iSCSI (TODO)

##### SCSI is a data access protocol for block devices.
##### iSCSI works on top of TCP. It header is 48 bytes (defining opcode to execute, LUN to connect etc) with variable byte iSCSI DATA. This allows SCSI command to be send over LAN or WAN. It does it by encapsulating SCSI command within its header and seamlessly sending over TCP. The connection is between iSCSI initiator and iSCSI server (also knows as target). When iSCSI packets arrives at the server, the iSCSI layer strips out the header and passes SCSI command to the block layer. For the server this is agnostic and it does not know if SCSI command is over network or local. 
##### iSCSI, protocol connection can be divided into 2 broad phases. First is login phase and second is Full Feature Phase or FFP.
##### Login phase, is mostly authenticate (CHAP etc) and if successful transfere to second phase. For this iSCSI inititor sends and PDU with opcode of LOGIN. Initiator also maintains a counter called CMDSN to detect any out of order dilivery to target.
##### Full feature phase is were DATA can be send and recieved from target. Target maintains a counter called STATSN to detect any out of order dilivery to initiator. Initiator maintains ExpectedSTATSN. Any differennce between ExpectedSTATSN and STATSN means that there is a missing command from target to initiator. 
##### Finally, for long running command (example large IO) - Initiator maintains an unique tag called (ITT or Initiator task tag) which remains same for long running IO.

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

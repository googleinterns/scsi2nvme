[![CI Status](https://github.com/googleinterns/scsi2nvme/workflows/CI/badge.svg?branch=master)](https://github.com/googleinterns/scsi2nvme/actions?query=branch%3Amaster)

# scsi2nvme

Tools for implementing SCSI on top of NVMe

This repo contains the following:
1) SCSI mock device
1) NVMe device driver
1) Translation Library
1) Engine to orchestrate execution flow

## Translation Library ##

The Translation Library offers a two-way command agnostic translation for SCSI commands to NVMe commands and NVMe responses to SCSI responses. It can be used in the user space or the kernel space.

The Library encapsulates away non-caller-actionable issues from the caller. For instance, since the caller is likely to be passing along a pre-constructed SCSI command to the Library, any issues with malformed SCSI commands is not actionable for the caller. In that case, the Library directly writes an error status to the SCSI buffer and no action is needed by the caller. 

In cases where the caller is using the Translation Library incorrectly, e.g. calling `Translation::Complete` before calling `Translation::Begin` or calling `Translation::Begin` twice in a row, the library returns an `ApiStatus` which currently is either `kSuccess` or `kFailure`. 

### Translation Interface ###
The Translation Interface provides 4 functions:

```
BeginResponse Begin(Span<const uint8_t> scsi_cmd, Span<const uint8_t> buffer, scsi::LunAddress lun);

Span<const NvmeCmdWrappers> GetNvmeWrappers();

CompleteReponse Complete(Span<const nvme::GenericQueueEntryCpl> cpl_data, Span<const uint8_t> buffer_in, Span<const uint8_t> sense_buffer);

void AbortPipeline(); 
```
### Translation::Begin() ###
```
BeginReponse Begin(Span<const uint8_t> scsi_cmd, 
                   Span<const uint8_t> buffer, 
                   scsi::LunAddress lun)
```

This function takes in:
- Raw SCSI command in bytes
- Buffer which can either be an input buffer or an output buffer. As an input buffer, it will be where the data from the device is read into. As an output buffer, it stores variable-length command data or the data to be written to the device
- Lun Address which is the NSID in NVMe

From the parameters, the function then delegates to the appropriate command translator and then builds 1 or more NVMe commands. For some commands such as TestUnitReady or Report Supported Op Codes, the Translation Library does not build an NVMe command and instead writes data directly to the SCSI buffer.

The functions returns:
```
BeginReponse {
  ApiStatus status;   // Indicates correct usage of Translation Library 
  uint32_t alloc_len; // Defines size of buffer passed to Translation::Complete
}
```
### Translation::GetNvmeWrappers() ###
```
Span<NvmeCmdWrapper> GetNvmeWrappers()
```
This function returns the constructed NVMe commands along with other useful data.

```
NvmeCmdWrapper {
  nvme::GenericQueueEntryCmd cmd; // NVMe command
  uint32_t buffer_len; // length of buffer pointed to by the NVMe PRP pointer
  bool is_admin; // whether command is an admin command. Needed for sending to correct NVMe queue
}
```

### Translation::Complete() ###
```
CompleteReponse Complete(Span<const nvme::GenericQueueEntryCpl> cpl_data, 
                         Span<const uint8_t> buffer_in,
                         Span<const uint8_t> sense_buffer);
```
The function takes in:
- NVMe Completion Queue Entry, which contains the NVMe response data
- SCSI data in buffer where response data is written to
- SCSI sense buffer where status codes are written to

This function translates the NVMe response data and status codes and writes them to the appropriate buffer.

### Translation::AbortPipeline ###
```
void AbortPipeline()
```
Finally, in the case that the Translation pipeline needs to be aborted, this function handles all the necesssary memory cleanup.

### Intended Usage ###
1. Get the Raw SCSI command and other data from the SCSI subsystem
1. Pass data to Translation::Begin()
1. Get constructed NVMe commands and other data with Translation::GetNvmeWrappers()
1. Send the commands to the NVMe device for execution
1. Pass the NVMe completion queue entries to Translation::Complete()

## End-to-end translation ##

### Setup ###
1) Ensure the following:
    - an NVMe device is attached to the machine at /dev/nvme0n1
    - kernel version is >= 4.19
1) Call `$ make` in the root directory. Ensure this results in a scsi2nvme.ko
1) Call `$ insmod scsi2nvme.ko` to insert the kernel module.

    Upon inserting, the SCSI mock device will be automatically attached. Once the device is attached, the SCSI subsystem will issue a set of commands. 

    `$ lsscsi` should show the device with the name "NVMe &nbsp; &nbsp;" (NVMe followed by 4 spaces)

### Issuing commands ###
Issue commands to the SCSI device as such:

Read: 
>`$ sudo dd if=$DEVICE bs=$BLOCK_SIZE count=$NUM_BLOCKS skip=$STARTING_BLOCK_ADDRESS`

Write:
>`$ sudo dd of=$DEVICE bs=$BLOCK_SIZE count=$NUM_BLOCKS seek=$STARTING_BLOCK_ADDRESS`

Then you will be prompted to enter the data to be written in stdin. Data can simply be a string of alphanumerical characters such as "The quick brown fox jumps over the lazy dog"


Issue commands to the NVMe device with the [nvme-cli](https://nvmexpress.org/open-source-nvme-management-utility-nvme-command-line-interface-nvme-cli)
Some examples are:
> `$ sudo nvme read /dev/nvme0n1/-z $BLOCK_SIZE -c $NUM_BLOCKS -s $STARTING_BLOCK_ADDRESS`

> `$ sudo nvme write /dev/nvme0n1/-z $BLOCK_SIZE -c $NUM_BLOCKS -s $STARTING_BLOCK_ADDRESS`


### See logs ###
 See logs with `$ sudo dmesg`

## Disclaimer

**This is not an officially supported Google product.**

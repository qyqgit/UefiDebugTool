/*
 * HdList.c
 *
 *  Created on: 2017Äê9ÔÂ14ÈÕ
 *      Author: red
 */


#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Protocol/DiskInfo.h>
#include "UefiShellDebug1CommandsLib/UefiShellDebug1CommandsLib.h"

///
/// ATA_IDENTIFY_DATA strictly complies with ATA/ATAPI-8 Spec
/// to define the data returned by an ATA device upon successful
/// completion of the ATA IDENTIFY_DEVICE command.
///
typedef struct {
  UINT16  config;                                  ///< General Configuration.
  UINT16  obsolete_1;
  UINT16  specific_config;                         ///< Specific Configuration.
  UINT16  obsolete_3;
  UINT16  retired_4_5[2];
  UINT16  obsolete_6;
  UINT16  cfa_reserved_7_8[2];
  UINT16  retired_9;
  CHAR8   SerialNo[20];                            ///< word 10~19
  UINT16  retired_20_21[2];
  UINT16  obsolete_22;
  CHAR8   FirmwareVer[8];                          ///< word 23~26
  CHAR8   ModelName[40];                           ///< word 27~46
  UINT16  multi_sector_cmd_max_sct_cnt;
  UINT16  trusted_computing_support;
  UINT16  capabilities_49;
  UINT16  capabilities_50;
  UINT16  obsolete_51_52[2];
  UINT16  field_validity;
  UINT16  obsolete_54_58[5];
  UINT16  multi_sector_setting;
  UINT16  user_addressable_sectors_lo;
  UINT16  user_addressable_sectors_hi;
  UINT16  obsolete_62;
  UINT16  multi_word_dma_mode;
  UINT16  advanced_pio_modes;
  UINT16  min_multi_word_dma_cycle_time;
  UINT16  rec_multi_word_dma_cycle_time;
  UINT16  min_pio_cycle_time_without_flow_control;
  UINT16  min_pio_cycle_time_with_flow_control;
  UINT16  additional_supported;                    ///< word 69
  UINT16  reserved_70;
  UINT16  reserved_71_74[4];                       ///< Reserved for IDENTIFY PACKET DEVICE cmd.
  UINT16  queue_depth;
  UINT16  serial_ata_capabilities;
  UINT16  reserved_77;                             ///< Reserved for Serial ATA
  UINT16  serial_ata_features_supported;
  UINT16  serial_ata_features_enabled;
  UINT16  major_version_no;
  UINT16  minor_version_no;
  UINT16  command_set_supported_82;                ///< word 82
  UINT16  command_set_supported_83;                ///< word 83
  UINT16  command_set_feature_extn;                ///< word 84
  UINT16  command_set_feature_enb_85;              ///< word 85
  UINT16  command_set_feature_enb_86;              ///< word 86
  UINT16  command_set_feature_default;             ///< word 87
  UINT16  ultra_dma_mode;                          ///< word 88
  UINT16  time_for_security_erase_unit;
  UINT16  time_for_enhanced_security_erase_unit;
  UINT16  advanced_power_management_level;
  UINT16  master_password_identifier;
  UINT16  hardware_configuration_test_result;
  UINT16  obsolete_94;
  UINT16  stream_minimum_request_size;
  UINT16  streaming_transfer_time_for_dma;
  UINT16  streaming_access_latency_for_dma_and_pio;
  UINT16  streaming_performance_granularity[2];    ///< word 98~99
  UINT16  maximum_lba_for_48bit_addressing[4];     ///< word 100~103
  UINT16  streaming_transfer_time_for_pio;
  UINT16  max_no_of_512byte_blocks_per_data_set_cmd;
  UINT16  phy_logic_sector_support;                ///< word 106
  UINT16  interseek_delay_for_iso7779;
  UINT16  world_wide_name[4];                      ///< word 108~111
  UINT16  reserved_for_128bit_wwn_112_115[4];
  UINT16  reserved_for_technical_report;
  UINT16  logic_sector_size_lo;                    ///< word 117
  UINT16  logic_sector_size_hi;                    ///< word 118
  UINT16  features_and_command_sets_supported_ext; ///< word 119
  UINT16  features_and_command_sets_enabled_ext;   ///< word 120
  UINT16  reserved_121_126[6];
  UINT16  obsolete_127;
  UINT16  security_status;                         ///< word 128
  UINT16  vendor_specific_129_159[31];
  UINT16  cfa_power_mode;                          ///< word 160
  UINT16  reserved_for_compactflash_161_167[7];
  UINT16  device_nominal_form_factor;
  UINT16  is_data_set_cmd_supported;
  CHAR8   additional_product_identifier[8];
  UINT16  reserved_174_175[2];
  CHAR8   media_serial_number[60];                 ///< word 176~205
  UINT16  sct_command_transport;                   ///< word 206
  UINT16  reserved_207_208[2];
  UINT16  alignment_logic_in_phy_blocks;           ///< word 209
  UINT16  write_read_verify_sector_count_mode3[2]; ///< word 210~211
  UINT16  verify_sector_count_mode2[2];
  UINT16  nv_cache_capabilities;
  UINT16  nv_cache_size_in_logical_block_lsw;      ///< word 215
  UINT16  nv_cache_size_in_logical_block_msw;      ///< word 216
  UINT16  nominal_media_rotation_rate;
  UINT16  reserved_218;
  UINT16  nv_cache_options;                        ///< word 219
  UINT16  write_read_verify_mode;                  ///< word 220
  UINT16  reserved_221;
  UINT16  transport_major_revision_number;
  UINT16  transport_minor_revision_number;
  UINT16  reserved_224_229[6];
  UINT64  extended_no_of_addressable_sectors;
  UINT16  min_number_per_download_microcode_mode3; ///< word 234
  UINT16  max_number_per_download_microcode_mode3; ///< word 235
  UINT16  reserved_236_254[19];
  UINT16  integrity_word;
} ATA_IDENTIFY_DATA;


INTN//£¿
EFIAPI
HdList (
  )
{
  EFI_STATUS                      Status;
  EFI_DISK_INFO_PROTOCOL          *DiskInfoProtocol;
  UINT8         *IdentifyData     = NULL;
  UINT32        DataSize          = 0;
  ATA_IDENTIFY_DATA *Stru         = NULL;
  UINTN        HandleBufferSize   = 0;
  EFI_HANDLE    *HandleBuffer     = NULL;
  UINTN         Index             = 0;


  Status = gBS->LocateHandleBuffer (
      ByProtocol,
      &gEfiDiskInfoProtocolGuid,
      NULL,
      &HandleBufferSize,
      &HandleBuffer);
  //Print (L"LocateHandleBuffer:%r\r\n", Status);
  Print(L"-----------------------------------------------\r\n");
  Print (L"number of disk:%x\r\n", HandleBufferSize);
  for(Index; Index < HandleBufferSize; Index++) {
    Print (L"%x / %x\r\n", Index + 1, HandleBufferSize);
    Status = gBS->HandleProtocol(HandleBuffer[Index],
        &gEfiDiskInfoProtocolGuid,
        (VOID**)&DiskInfoProtocol);
    //Print (L"HandleProtocol:%r\r\n", Status);
    Status = DiskInfoProtocol->Identify(DiskInfoProtocol, IdentifyData, &DataSize);
    //Print (L"Identify:%r\r\n", Status);
    if (Status == EFI_NOT_FOUND) {
      Print (L"identify data not found.\r\n");
      return Status;
    }
    if (Status == EFI_BUFFER_TOO_SMALL) {

      IdentifyData = AllocatePool ( DataSize);
      if (IdentifyData == NULL)
        return EFI_OUT_OF_RESOURCES;

      Status = DiskInfoProtocol->Identify (DiskInfoProtocol, IdentifyData, &DataSize);
      //Print (L"Identify:%r\r\n", Status);
    }
    Stru = (ATA_IDENTIFY_DATA *)(IdentifyData);
    Print (L"SeriaNo    :%a\r\n", Stru->SerialNo);
    Print (L"FirmwareVer:%a\r\n", Stru->FirmwareVer);
    Print (L"ModleName  :%a\r\n", Stru->ModelName);
    DumpHex(2, 0, DataSize, IdentifyData);
    Print(L"-----------------------------------------------\r\n");
    ShellPromptForResponse (ShellPromptResponseTypeAnyKeyContinue, NULL, (VOID**)NULL);
  }

  return EFI_SUCCESS;

}

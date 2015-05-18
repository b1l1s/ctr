#ifndef __3DSHEADERS_H
#define __3DSHEADERS_H

#include "ctr/types.h"

#define FIRM_TYPE_ARM9 0
#define FIRM_TYPE_ARM11 1

#define MEDIA_UNITS 0x200

#define NCCH_MAGIC		(0x4843434E)
#define NCSD_MAGIC		(0x4453434E)
#define FIRM_MAGIC		(0x4D524946)

typedef struct firm_section_h
{
	u32 offset;
	void* address;
	u32 size;
	u32 type;		// Firmware Type ('0'=ARM9/'1'=ARM11)
	u8 hash[0x20];	// SHA-256 Hash of Firmware Section
} firm_section_h; // 0x30

typedef struct firm_h
{
	u32 magic; 		// FIRM
	u32 reserved1;
	void* a11Entry;	// ARM11 entry
	void* a9Entry;	// ARM9 entry
	u8 reserved2[0x30];
	firm_section_h section[4];
	u8 sig[0x100];
} firm_h;

// http://3dbrew.org/wiki/NCCH/Extended_Header
typedef struct code_set_info
{
	u32 address;
	u32 phy_region_size;	// Physical region size (in page-multiples)
	u32 size;				// size in bytes
} code_set_info; // 0x0C

typedef struct system_info
{
	u32 saveDataSize[2];
	u32 jumpID[2];
	u8 reserved[0x30];
} system_info; // 0x40

typedef struct
{
	u8 appTitle[8];
	char reserved1[5];
	char flag;
	char remasterVersion[2];
	code_set_info textCodeSet;
	u32 stackSize;
	code_set_info roCodeSet;
	char reserved2[4];
	code_set_info dataCodeSet;
	u32 bssSize;
	char depends[0x180]; // Dependency Module (Program ID) List 48*8
	system_info systemInfo;
} system_control_info; // 0x200

typedef struct ncch_ex_h
{
	system_control_info sci;
	u8 aci[0x200];
	u8 accessDescSig[0x100];
	u8 ncchPubKey[0x100];
	u8 aciLim[0x200];
} ncch_ex_h; // 0x800

// http://3dbrew.org/wiki/NCCH
typedef struct ncch_h
{
	u8 sig[0x100];		// RSA-2048 signature of the NCCH header, using SHA-256.
	u32 magic; 			// NCCH
	u32 contentSize;	// Content size, in media units (1 media unit = 0x200 bytes)
	u8 partitionID[8];
	u8 makerCode[2];
	u16 version;
	u8 reserved1[4];
	u8 programID[8];
	u8 reserved2[0x10];
	u8 logoHash[0x20];	// Logo Region SHA-256 hash. (For applications built with SDK 5+) (Supported from firmware: 5.0.0-11)
	u8 productCode[0x10];
	u8 exHeaderHash[0x20]; // Extended header SHA-256 hash (SHA256 of 2x Alignment Size, beginning at 0x0 of ExHeader)
	u32 exHeaderSize;	// Extended header size
	u32 reserved3;
	u8 flags[8];
	u32 plainOffset;	// media unit
	u32 plainSize;		// media unit
	u32 logoOffset;		// media unit
	u32 logoSize;		// media unit
	u32 exeFSOffset;	// media unit
	u32 exeFSSize;		// media unit
	u32 exeFSHashSize;	// media unit ExeFS hash region size
	u32 reserved4;
	u32 romFSOffset;	// media unit
	u32 romFSSize;		// media unit
	u32 romFSHashSize;	// media unit RomFS hash region size
	u32 reserved5;
	u8 exeFSHash[0x20];	// ExeFS superblock SHA-256 hash - (SHA-256 hash, starting at 0x0 of the ExeFS over the number of media units specified in the ExeFS hash region size)
	u8 romFSHash[0x20]; // RomFS superblock SHA-256 hash - (SHA-256 hash, starting at 0x0 of the RomFS over the number of media units specified in the RomFS hash region size)
} ncch_h; // 0x200

#define NCCH_FLAG_NOCRYPTO			0x4
#define NCCH_FLAG_7XCRYPTO			0x1

typedef struct cxi_h
{
	ncch_h ncch;
	ncch_ex_h exheader;
} cxi_h;

typedef struct exefs_file_h
{
	u8 fname[0x8];
	u32 offset; // offset starts after exefs_h
	u32 size;
} exefs_file_h; // 0x10

typedef struct exefs_file_hash
{
	u8 hash[0x20];
} exefs_file_hash;

typedef struct exefs_h
{
	exefs_file_h fileHeaders[10]; // File headers (10 headers maximum, 16 bytes each)
	u8 reserved[0x20];
	exefs_file_hash fileHashes[10]; // File hashes (10 hashes maximum, 32 bytes each, one for each header), SHA256 over entire file content
} exefs_h; // 0x200

typedef struct ncsd_partition_table
{
	u32 offset;
	u32 size;
} ncsd_partition_table;

// http://3dbrew.org/wiki/NCSD
typedef struct ncsd_h
{
	u8 sig[0x100];		// RSA-2048 signature of the NCSD header, using SHA-256.
	u32 magic; 			// NCSD
	u32 size;			// Size of the NCSD image, in media units (1 media unit = 0x200 bytes)
	u8 mediaID[8];
	u8 fsType[8];		// Partitions FS type (0=None, 1=Normal, 3=FIRM, 4=AGB_FIRM save)
	u8 cryptType[8];	// Partitions crypt type
	ncsd_partition_table ptable[8];	// Offset & Length partition table, in media units
	u8 spec[0xA0];		// 
} ncsd_h;

#define NCSD_PARTITION_EXE				0
#define NCSD_PARTITION_MANUAL			1
#define NCSD_PARTITION_DLP				2
#define NCSD_PARTITION_N3DSUPDATE		6
#define NCSD_PARTITION_O3DSUPDATE		7

// http://3dbrew.org/wiki/CIA
typedef struct cia_h
{
	u32 size;			// Size of the header (usually 0x2020)
	u16 type;
	u16 version;
	u32 certChainSize;
	u32 ticketSize;
	u32 tmdSize;
	u32 metaSize;
	u32 contentSize;
	u8 contentIndex[0x2000];
} cia_h;

#define SIG_TYPE_RSA4096_SHA1		0x010000
#define SIG_TYPE_RSA2048_SHA1		0x010001
#define SIG_TYPE_ECDSA_SHA1			0x010002
#define SIG_TYPE_RSA4096_SHA256		0x010003
#define SIG_TYPE_RSA2048_SHA256		0x010004
#define SIG_TYPE_ECDSA_SHA256		0x010005

typedef struct cia_sig_s
{
	u32 type;
	void* sig;
} cia_sig_s;

typedef struct tmd_content_info_rec_s
{
	u16 contentIndexOffset;
	u16 contentCommandCount;
	u8 contentChunkHash[0x20];
} __attribute__((__packed__)) tmd_content_info_rec_s;

typedef struct tmd_content_chunk_record_s
{
	u32 contentID;
	u16 contentIndex;
	u16 contentType;
	u64 contentSize;
	u8 contentHash[0x20];
} __attribute__((__packed__)) tmd_content_chunk_record_s;

// http://3dbrew.org/wiki/TMD
typedef struct tmd_h
{
	u8 sigIssuer[0x40];
	u8 version;
	u8 caCrlVersion;
	u8 signerCrlVersion;
	u8 reserved;
	u8 systemVersion[8];
	u8 titleID[8];
	u32 titleType;
	u8 groupID[2];
	u8 saveDataSize[4];
	u8 privateSaveDataSize[4];
	u8 reserved2[4];
	u8 twlFlag;
	u8 reserved3[0x31];
	u32 accessRights;
	u16 titleVersion;
	u16 contentCount;
	u16 bootContent;
	u16 pad;
	u8 contentInfoRecordsHash[0x20];
} __attribute__((__packed__)) tmd_h;

// http://3dbrew.org/wiki/Ticket
typedef struct ticket_h
{
	u8 sigIssuer[0x40];
	u8 eccPubKey[0x3C];
	u8 version;
	u8 caCrlVersion;
	u8 signerCrlVersion;
	u8 titleKey[0x10];
	u8 reserved;
	u8 ticketID[8];
	u8 consoleID[4];
	u8 titleID[8];
	u8 reserved2[2];
	u16 ticketTitleVersion;
	u8 reserved3[8];
	u8 licenseType;
	u8 ticketCommonKeyYIndex;	//Ticket common keyY index, usually 0x1 for retail system titles.
	u8 reserved4[0x2A];
	u8 unk[4];					// eShop Account ID?
	u8 reserved5;
	u8 audit;
	u8 reserved6[0x42];
	u8 limits[0x40];
	u8 contentIndex[0xAC];
} __attribute__((__packed__)) ticket_h; // 0x210

// http://3dbrew.org/wiki/Certificates
typedef struct cert_h
{
	u8 issuer[0x40];
	u32 keyType;
	u8 name[0x40];
	u32 unk;
} __attribute__((__packed__)) cert_h;

typedef struct cert_pkey_rsa4096_s
{
	u8 modulus[0x200];
	u32 exp;
	u8 pad[0x34];
} __attribute__((__packed__)) cert_pkey_rsa4096_s;

typedef struct cert_pkey_rsa2048_s
{
	u8 modulus[0x100];
	u32 exp;
	u8 pad[0x34];
} __attribute__((__packed__)) cert_pkey_rsa2048_s;

typedef struct cert_pkey_ecc_s
{
	u8 pkey[0x3C];
	u8 pad[0x3C];
} __attribute__((__packed__)) cert_pkey_ecc_s;

#endif /*__3DSHEADERS_H*/

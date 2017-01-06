#include "AEConfig.h"
#include "AE_EffectVers.h"

#ifndef AE_OS_WIN
	#include <AE_General.r>
#endif
	
resource 'PiPL' (16000) {
	{	/* array properties: 12 elements */
		/* [1] */
		Kind {
			AEEffect
		},
		/* [2] */
		Name {
			"XJ_Charter"
		},
		/* [3] */
		Category {
			"Generate"
		},
#ifdef AE_OS_WIN
	#ifdef AE_PROC_INTELx64
		CodeWin64X86 {"EntryPointFuncM"},
	#else
		CodeWin32X86 {"EntryPointFuncM"},
	#endif
#else
	#ifdef AE_OS_MAC
		CodeMachOPowerPC {"EntryPointFuncM"},
		CodeMacIntel32 {"EntryPointFuncM"},
		CodeMacIntel64 {"EntryPointFuncM"},
	#endif
#endif
		/* [6] */
		AE_PiPL_Version {
			2,
			0
		},
		/* [7] */
		AE_Effect_Spec_Version {
			PF_PLUG_IN_VERSION,
			PF_PLUG_IN_SUBVERS
		},
		/* [8] */
/*
RESOURCE_VERSION =
MAJOR_VERSION * 524288 +
MINOR_VERSION * 32768 +
BUG_VERSION * 2048 +
STAGE_VERSION * 512 +
BUILD_VERSION

*/
		AE_Effect_Version {
			525313	/* 1.0 */
		},
		/* [9] */
		AE_Effect_Info_Flags {
			0
		},
		/* [10] */
		AE_Effect_Global_OutFlags {
		0x00000000

		},
		AE_Effect_Global_OutFlags_2 {
		0x00000000
		},
		/* [11] */
		AE_Effect_Match_Name {
			"XJ_Charter"
		},
		/* [12] */
		AE_Reserved_Info {
			0
		}
	}
};

resource 'PiPL' (16001) {
	{	/* array properties: 12 elements */
		/* [1] */
		Kind {
			AEEffect
		},
		/* [2] */
		Name {
			"XJ_Charter Number"
		},
		/* [3] */
		Category {
			"Xiao Juan"
		},
#ifdef AE_OS_WIN
	#ifdef AE_PROC_INTELx64
		CodeWin64X86 {"EntryPointFuncN"},
	#else
		CodeWin32X86 {"EntryPointFuncN"},
	#endif
#else
	#ifdef AE_OS_MAC
		CodeMachOPowerPC {"EntryPointFuncN"},
		CodeMacIntel32 {"EntryPointFuncN"},
		CodeMacIntel64 {"EntryPointFuncN"},
	#endif
#endif
		/* [6] */
		AE_PiPL_Version {
			2,
			0
		},
		/* [7] */
		AE_Effect_Spec_Version {
			PF_PLUG_IN_VERSION,
			PF_PLUG_IN_SUBVERS
		},
		/* [8] */
/*
RESOURCE_VERSION =
MAJOR_VERSION * 524288 +
MINOR_VERSION * 32768 +
BUG_VERSION * 2048 +
STAGE_VERSION * 512 +
BUILD_VERSION

*/
		AE_Effect_Version {
			525313	/* 1.0 */
		},
		/* [9] */
		AE_Effect_Info_Flags {
			0
		},
		/* [10] */
		AE_Effect_Global_OutFlags {
		0x00200000 //50332160

		},
		AE_Effect_Global_OutFlags_2 {
		0x00000000
		},
		/* [11] */
		AE_Effect_Match_Name {
			"XJ_CharterNUM"
		},
		/* [12] */
		AE_Reserved_Info {
			0
		}
	}
};


resource 'PiPL' (16002) {
	{	/* array properties: 12 elements */
		/* [1] */
		Kind {
			AEEffect
		},
		/* [2] */
		Name {
			"XJ_Charter Bar Render"
		},
		/* [3] */
		Category {
			"Xiao Juan"
		},
#ifdef AE_OS_WIN
	#ifdef AE_PROC_INTELx64
		CodeWin64X86 {"EntryPointFuncB"},
	#else
		CodeWin32X86 {"EntryPointFuncB"},
	#endif
#else
	#ifdef AE_OS_MAC
		CodeMachOPowerPC {"EntryPointFuncB"},
		CodeMacIntel32 {"EntryPointFuncB"},
		CodeMacIntel64 {"EntryPointFuncB"},
	#endif
#endif
		/* [6] */
		AE_PiPL_Version {
			2,
			0
		},
		/* [7] */
		AE_Effect_Spec_Version {
			PF_PLUG_IN_VERSION,
			PF_PLUG_IN_SUBVERS
		},
		/* [8] */
/*
RESOURCE_VERSION =
MAJOR_VERSION * 524288 +
MINOR_VERSION * 32768 +
BUG_VERSION * 2048 +
STAGE_VERSION * 512 +
BUILD_VERSION

*/
		AE_Effect_Version {
			525313	/* 1.0 */
		},
		/* [9] */
		AE_Effect_Info_Flags {
			0
		},
		/* [10] */
		AE_Effect_Global_OutFlags {
		0x00200004 //50332160

		},
		AE_Effect_Global_OutFlags_2 {
		0x00000000
		},
		/* [11] */
		AE_Effect_Match_Name {
			"XJ_CharterBARR"
		},
		/* [12] */
		AE_Reserved_Info {
			0
		}
	}
};

resource 'PiPL' (16003) {
	{	/* array properties: 12 elements */
		/* [1] */
		Kind {
			AEEffect
		},
		/* [2] */
		Name {
			"XJ_Charter Pie Render"
		},
		/* [3] */
		Category {
			"Xiao Juan"
		},
#ifdef AE_OS_WIN
	#ifdef AE_PROC_INTELx64
		CodeWin64X86 {"EntryPointFuncR"},
	#else
		CodeWin32X86 {"EntryPointFuncR"},
	#endif
#else
	#ifdef AE_OS_MAC
		CodeMachOPowerPC {"EntryPointFuncR"},
		CodeMacIntel32 {"EntryPointFuncR"},
		CodeMacIntel64 {"EntryPointFuncR"},
	#endif
#endif
		/* [6] */
		AE_PiPL_Version {
			2,
			0
		},
		/* [7] */
		AE_Effect_Spec_Version {
			PF_PLUG_IN_VERSION,
			PF_PLUG_IN_SUBVERS
		},
		/* [8] */
/*
RESOURCE_VERSION =
MAJOR_VERSION * 524288 +
MINOR_VERSION * 32768 +
BUG_VERSION * 2048 +
STAGE_VERSION * 512 +
BUILD_VERSION

*/
		AE_Effect_Version {
			525313	/* 1.0 */
		},
		/* [9] */
		AE_Effect_Info_Flags {
			0
		},
		/* [10] */
		AE_Effect_Global_OutFlags {
		0x00200004 //50332160

		},
		AE_Effect_Global_OutFlags_2 {
		0x00000000
		},
		/* [11] */
		AE_Effect_Match_Name {
			"XJ_CharterRINGR"
		},
		/* [12] */
		AE_Reserved_Info {
			0
		}
	}
};

resource 'PiPL' (16004) {
	{	/* array properties: 12 elements */
		/* [1] */
		Kind {
			AEEffect
		},
		/* [2] */
		Name {
			"XJ_Charter Linker"
		},
		/* [3] */
		Category {
			"Xiao Juan"
		},
#ifdef AE_OS_WIN
	#ifdef AE_PROC_INTELx64
		CodeWin64X86 {"EntryPointFuncLINK"},
	#else
		CodeWin32X86 {"EntryPointFuncLINK"},
	#endif
#else
	#ifdef AE_OS_MAC
		CodeMachOPowerPC {"EntryPointFuncLINK"},
		CodeMacIntel32 {"EntryPointFuncLINK"},
		CodeMacIntel64 {"EntryPointFuncLINK"},
	#endif
#endif
		/* [6] */
		AE_PiPL_Version {
			2,
			0
		},
		/* [7] */
		AE_Effect_Spec_Version {
			PF_PLUG_IN_VERSION,
			PF_PLUG_IN_SUBVERS
		},
		/* [8] */
/*
RESOURCE_VERSION =
MAJOR_VERSION * 524288 +
MINOR_VERSION * 32768 +
BUG_VERSION * 2048 +
STAGE_VERSION * 512 +
BUILD_VERSION

*/
		AE_Effect_Version {
			525313	/* 1.0 */
		},
		/* [9] */
		AE_Effect_Info_Flags {
			0
		},
		/* [10] */
		AE_Effect_Global_OutFlags {
		0x00200000 //50332160

		},
		AE_Effect_Global_OutFlags_2 {
		0x00000000
		},
		/* [11] */
		AE_Effect_Match_Name {
			"XJ_CharterLINK"
		},
		/* [12] */
		AE_Reserved_Info {
			0
		}
	}
};

resource 'PiPL' (16005) {
	{	/* array properties: 12 elements */
		/* [1] */
		Kind {
			AEEffect
		},
		/* [2] */
		Name {
			"XJ_Charter Line Render"
		},
		/* [3] */
		Category {
			"Xiao Juan"
		},
#ifdef AE_OS_WIN
	#ifdef AE_PROC_INTELx64
		CodeWin64X86 {"EntryPointFuncL"},
	#else
		CodeWin32X86 {"EntryPointFuncL"},
	#endif
#else
	#ifdef AE_OS_MAC
		CodeMachOPowerPC {"EntryPointFuncL"},
		CodeMacIntel32 {"EntryPointFuncL"},
		CodeMacIntel64 {"EntryPointFuncL"},
	#endif
#endif
		/* [6] */
		AE_PiPL_Version {
			2,
			0
		},
		/* [7] */
		AE_Effect_Spec_Version {
			PF_PLUG_IN_VERSION,
			PF_PLUG_IN_SUBVERS
		},
		/* [8] */
/*
RESOURCE_VERSION =
MAJOR_VERSION * 524288 +
MINOR_VERSION * 32768 +
BUG_VERSION * 2048 +
STAGE_VERSION * 512 +
BUILD_VERSION

*/
		AE_Effect_Version {
			525313	/* 1.0 */
		},
		/* [9] */
		AE_Effect_Info_Flags {
			0
		},
		/* [10] */
		AE_Effect_Global_OutFlags {
		0x00200404

		},
		AE_Effect_Global_OutFlags_2 {
		0x00040008
		},
		/* [11] */
		AE_Effect_Match_Name {
			"XJ_CharterLINER"
		},
		/* [12] */
		AE_Reserved_Info {
			0
		}
	}
};
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
			"Stroke Maskter(BETA)"
		},
		/* [3] */
		Category {
			"Generate"
		},
#ifdef AE_OS_WIN
	#ifdef AE_PROC_INTELx64
		CodeWin64X86 {"EntryPointFunc"},
	#else
		CodeWin32X86 {"EntryPointFunc"},
	#endif
#else
	#ifdef AE_OS_MAC
		CodeMachOPowerPC {"EntryPointFunc"},
		CodeMacIntel32 {"EntryPointFunc"},
		CodeMacIntel64 {"EntryPointFunc"},
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
		0x00000000 //50332160

		},
		AE_Effect_Global_OutFlags_2 {
		0x00000008
		},
		/* [11] */
		AE_Effect_Match_Name {
			"Stroke Maskter"
		},
		/* [12] */
		AE_Reserved_Info {
			0
		}
	}
};


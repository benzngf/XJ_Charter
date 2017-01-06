/*******************************************************************/
/*                                                                 */
/*                      ADOBE CONFIDENTIAL                         */
/*                   _ _ _ _ _ _ _ _ _ _ _ _ _                     */
/*                                                                 */
/* Copyright 2007 Adobe Systems Incorporated                       */
/* All Rights Reserved.                                            */
/*                                                                 */
/* NOTICE:  All information contained herein is, and remains the   */
/* property of Adobe Systems Incorporated and its suppliers, if    */
/* any.  The intellectual and technical concepts contained         */
/* herein are proprietary to Adobe Systems Incorporated and its    */
/* suppliers and may be covered by U.S. and Foreign Patents,       */
/* patents in process, and are protected by trade secret or        */
/* copyright law.  Dissemination of this information or            */
/* reproduction of this material is strictly forbidden unless      */
/* prior written permission is obtained from Adobe Systems         */
/* Incorporated.                                                   */
/*                                                                 */
/*******************************************************************/

/*	XJ_Charter.cpp

This is a compiling husk of a project. Fill it in with interesting
pixel processing code.

Revision history:

1.0 (seemed like a good idea at the time)	bbb		6/1/2002

1.0 Okay, I'm leaving the version at 1.0,	bbb		2/15/2006
for obvious reasons; you're going to
copy these files directly! This is the
first XCode version, though.

1.0	Let's simplify this barebones sample	zal		11/11/2010

*/

#include "XJ_Charter.h"
#define	FLOAT2FIX2(F)			((PF_Fixed)((F) * 65536 + (((F) < 0) ? -1 : 1)))
static PF_Err
About_N(
	PF_InData		*in_data,
	PF_OutData		*out_data,
	PF_ParamDef		*params[],
	PF_LayerDef		*output)
{
	AEGP_SuiteHandler suites(in_data->pica_basicP);

	suites.ANSICallbacksSuite1()->sprintf(out_data->return_msg,
		"Hi :3 \n\nFor any questions, bug report\nor if you have some ideas about new features,\nplease contact me via e-mail\n  benzngf@gmail.com  \nThanks~~");
	return PF_Err_NONE;
}

static PF_Err
GlobalSetup_N(
	PF_InData		*in_data,
	PF_OutData		*out_data,
	PF_ParamDef		*params[],
	PF_LayerDef		*output)
{
	out_data->my_version = PF_VERSION(MAJOR_VERSION,
		MINOR_VERSION,
		BUG_VERSION,
		STAGE_VERSION,
		BUILD_VERSION);
	out_data->out_flags = PF_OutFlag_I_AM_OBSOLETE;
	out_data->out_flags2 = PF_OutFlag2_NONE;

	return PF_Err_NONE;
}

enum PARAMS_N {
	NUMBER_SLIDER,
	COLOR,
	LAYER_DEF
};

static PF_Err
ParamsSetup_N(
	PF_InData		*in_data,
	PF_OutData		*out_data,
	PF_ParamDef		*params[],
	PF_LayerDef		*output)
{
	PF_Err		err = PF_Err_NONE;
	PF_ParamDef	def;

	AEFX_CLR_STRUCT(def);
	out_data->num_params = 1;

	def.param_type = PF_Param_FLOAT_SLIDER;
	PF_STRCPY(def.name, "Number Value");
	def.uu.id = 4;
	def.u.fs_d.precision = 1;
	def.u.fs_d.dephault = 100;
	def.u.fs_d.slider_min = 0;
	def.u.fs_d.slider_max = 200;
	def.u.fs_d.valid_max = 36000000;
	def.u.fs_d.valid_min = 0;
	def.u.fs_d.display_flags = 0;
	//if (params[MODEPOPUP]->u.pd.value == 1) def.ui_flags = PF_PUI_DISABLED;
	if (err = PF_ADD_PARAM(in_data, -1, &def))
		return err;
	AEFX_CLR_STRUCT(def);
	out_data->num_params++;

	def.param_type = PF_Param_COLOR;
	PF_STRCPY(def.name, "Color");
	def.u.cd.dephault.alpha = 255;
	def.u.cd.dephault.red = 255;
	def.u.cd.dephault.blue = 0;
	def.u.cd.dephault.green = 0;
	def.u.cd.value = def.u.cd.dephault;
	def.uu.id = 2;
	def.flags = PF_ParamFlag_NONE;
	if (err = PF_ADD_PARAM(in_data, -1, &def))
		return err;
	AEFX_CLR_STRUCT(def);
	out_data->num_params++;

	//Not yet used
	/*def.param_type = PF_Param_LAYER;
	PF_STRCPY(def.name, "Choose emit layer");
	def.uu.id = 3;
	def.flags = PF_ParamFlag_NONE;
	if (err = PF_ADD_PARAM(in_data, -1, &def))
		return err;
	AEFX_CLR_STRUCT(def);
	out_data->num_params++;*/


	return err;
}


DllExport
PF_Err
EntryPointFuncN (
	PF_Cmd			cmd,
	PF_InData		*in_data,
	PF_OutData		*out_data,
	PF_ParamDef		*params[],
	PF_LayerDef		*output,
	void			*extra)
{
	PF_Err		err = PF_Err_NONE;

	try {
		switch (cmd) {
		case PF_Cmd_ABOUT:

			err = About_N(in_data,
				out_data,
				params,
				output);
			break;

		case PF_Cmd_GLOBAL_SETUP:

			err = GlobalSetup_N(in_data,
				out_data,
				params,
				output);
			break;

		case PF_Cmd_PARAMS_SETUP:

			err = ParamsSetup_N(in_data,
				out_data,
				params,
				output);
			break;

		/*case PF_Cmd_FRAME_SETUP:
			out_data->out_flags |= PF_OutFlag_NOP_RENDER;
			err = PF_Err_NONE;
			break;*/
		case PF_Cmd_RENDER:
			AEGP_SuiteHandler suites(in_data->pica_basicP);
			suites.WorldTransformSuite1()->copy_hq(in_data->effect_ref, &params[0]->u.ld, output, NULL, &output->extent_hint);
			//*output = params[0]->u.ld;
			break;
		}
	}
	catch (PF_Err &thrown_err) {
		err = thrown_err;
	}
	return err;
}


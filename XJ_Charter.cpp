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

static int about_count = 0;
static PF_Err 
About (	
	PF_InData		*in_data,
	PF_OutData		*out_data,
	PF_ParamDef		*params[],
	PF_LayerDef		*output )
{
	AEGP_SuiteHandler suites(in_data->pica_basicP);
	
	suites.ANSICallbacksSuite1()->sprintf(	out_data->return_msg,
											"Hi :3 \n\nFor any questions, bug report\nor if you have some ideas about new features,\nplease contact me via e-mail\n  benzngf@gmail.com  \nThanks~~");
	about_count++;
	return PF_Err_NONE;
}
static AEGP_PluginID myAEGPID;
static AEGP_InstalledEffectKey key_NUM = 0, key_RENBAR = 0, key_RENRING = 0, key_LINK = 0, key_RENLINE = 0;
static PF_Err 
GlobalSetup (	
	PF_InData		*in_data,
	PF_OutData		*out_data,
	PF_ParamDef		*params[],
	PF_LayerDef		*output )
{
	out_data->my_version = PF_VERSION(	MAJOR_VERSION, 
										MINOR_VERSION,
										BUG_VERSION, 
										STAGE_VERSION, 
										BUILD_VERSION);
	out_data->out_flags = PF_OutFlag_NONE;
	out_data->out_flags2 = PF_OutFlag2_NONE;
	AEGP_SuiteHandler suites(in_data->pica_basicP);
	suites.UtilitySuite5()->AEGP_RegisterWithAEGP(NULL, out_data->name,&myAEGPID);
	AEGP_InstalledEffectKey keyTMP = AEGP_InstalledEffectKey_NONE;
	/*FIND THE KEY FOR THE EFFECTS!!!!*/
	A_long Total_effects_count;
	suites.EffectSuite3()->AEGP_GetNumInstalledEffects(&Total_effects_count);
	A_long now = 0;
	A_char matchNameTMP[AEGP_MAX_EFFECT_MATCH_NAME_SIZE + 1];
	A_char name_NUM[AEGP_MAX_EFFECT_MATCH_NAME_SIZE + 1], name_RENBAR[AEGP_MAX_EFFECT_MATCH_NAME_SIZE + 1], 
		name_RENRING[AEGP_MAX_EFFECT_MATCH_NAME_SIZE + 1], name_LINK[AEGP_MAX_EFFECT_MATCH_NAME_SIZE + 1],
		name_LINE[AEGP_MAX_EFFECT_MATCH_NAME_SIZE + 1];
	PF_STRCPY(name_NUM,"XJ_CharterNUM");
	PF_STRCPY(name_RENBAR, "XJ_CharterBARR");
	PF_STRCPY(name_RENRING, "XJ_CharterRINGR");
	PF_STRCPY(name_LINK, "XJ_CharterLINK");
	PF_STRCPY(name_LINE, "XJ_CharterLINER");
	while (now < Total_effects_count) {
		suites.EffectSuite3()->AEGP_GetNextInstalledEffect(keyTMP, &keyTMP);
		suites.EffectSuite3()->AEGP_GetEffectMatchName(keyTMP, matchNameTMP);
		if (!strcmp(name_NUM, matchNameTMP)) {
			key_NUM = keyTMP;
		}
		else if (!strcmp(name_RENBAR, matchNameTMP)) {
			key_RENBAR = keyTMP;
		}
		else if (!strcmp(name_RENRING, matchNameTMP)) {
			key_RENRING = keyTMP;
		}
		else if (!strcmp(name_LINK, matchNameTMP)) {
			key_LINK = keyTMP;
		}
		else if (!strcmp(name_LINE, matchNameTMP)) {
			key_RENLINE = keyTMP;
		}
		if ((key_NUM != 0) && (key_RENBAR != 0) && (key_RENRING != 0) && (key_LINK != 0) && (key_RENLINE != 0)) {
			break;
		}
		now++;
	}
	return PF_Err_NONE;
}

enum PARAMS{
	RENDERTYPE_POPUP = 1,
	CREATERENDERER_BTN,
	CREATENUM_BTN,
	CREATELINKER_BTN
};

static PF_Err 
ParamsSetup (	
	PF_InData		*in_data,
	PF_OutData		*out_data,
	PF_ParamDef		*params[],
	PF_LayerDef		*output )
{
	PF_Err		err		= PF_Err_NONE;
	PF_ParamDef	def;	

	AEFX_CLR_STRUCT(def);
	out_data->num_params = 1;

	A_char PopupStr_1[100];
	PF_STRCPY(PopupStr_1, "Bar Chart|Line Chart|Pie Chart");
	def.param_type = PF_Param_POPUP;
	def.flags = PF_ParamFlag_CANNOT_TIME_VARY;
	PF_STRCPY(def.name, "Choose Chart Type :");
	def.uu.id = 1;
	def.u.pd.num_choices = 3;
	def.u.pd.dephault = 1;
	def.u.pd.value = def.u.pd.dephault;
	def.u.pd.u.namesptr = PopupStr_1;
	if (err = PF_ADD_PARAM(in_data, -1, &def))
		return err;
	AEFX_CLR_STRUCT(def);
	out_data->num_params++;

	def.param_type = PF_Param_BUTTON;
	PF_STRCPY(def.name, "Create Chart");
	def.u.button_d.u.namesptr = def.name;
	def.flags |= PF_ParamFlag_SUPERVISE;
	def.uu.id = 2;
	def.flags |= PF_ParamFlag_CANNOT_TIME_VARY;
	if (err = PF_ADD_PARAM(in_data, -1, &def))
		return err;
	AEFX_CLR_STRUCT(def);
	out_data->num_params++;

	def.param_type = PF_Param_BUTTON;
	PF_STRCPY(def.name, "Create Number");
	def.u.button_d.u.namesptr = def.name;
	def.flags |= PF_ParamFlag_SUPERVISE;
	def.uu.id = 3;
	def.flags |= PF_ParamFlag_CANNOT_TIME_VARY;
	if (err = PF_ADD_PARAM(in_data, -1, &def))
		return err;
	AEFX_CLR_STRUCT(def);
	out_data->num_params++;

	def.param_type = PF_Param_BUTTON;
	PF_STRCPY(def.name, "Create Linker");
	def.u.button_d.u.namesptr = def.name;
	def.flags |= PF_ParamFlag_SUPERVISE;
	def.uu.id = 4;
	def.flags |= PF_ParamFlag_CANNOT_TIME_VARY;
	if (err = PF_ADD_PARAM(in_data, -1, &def))
		return err;
	AEFX_CLR_STRUCT(def);
	out_data->num_params++;

	return err;
}


static int develop_count = 0;

static PF_Err
ChangeParam(
PF_InData		*in_data,
PF_ParamDef		*params[],
PF_OutData  *out_data,
PF_LayerDef		*output,
PF_UserChangedParamExtra *extra){

	AEGP_SuiteHandler suites(in_data->pica_basicP);
	if (extra->param_index == CREATERENDERER_BTN){
		AEGP_LayerH LayerH;
		AEGP_EffectRefH EffectRefH;
		switch (params[RENDERTYPE_POPUP]->u.pd.value){
		case 1:	
			suites.PFInterfaceSuite1()->AEGP_GetEffectLayer(in_data->effect_ref, &LayerH);
			suites.EffectSuite3()->AEGP_ApplyEffect(myAEGPID, LayerH, key_RENBAR, &EffectRefH);
			suites.EffectSuite3()->AEGP_DisposeEffect(EffectRefH);
			break;
		case 2:
			/*if (develop_count < 3) {
				suites.ANSICallbacksSuite1()->sprintf(out_data->return_msg, "折線圖功能尚在開發中");
			}else if(develop_count < 4){
				suites.ANSICallbacksSuite1()->sprintf(out_data->return_msg, "就跟你說還沒好嘛，有耐心一點啦");
			}else if (develop_count < 8) {
				suites.ANSICallbacksSuite1()->sprintf(out_data->return_msg, "都這麼說了你還按！");
			}else {
				suites.ANSICallbacksSuite1()->sprintf(out_data->return_msg, "……不管你了啦，這麼想要、就、給你吧、哼！");
				
			}
			develop_count++;*/
			suites.PFInterfaceSuite1()->AEGP_GetEffectLayer(in_data->effect_ref, &LayerH);
			suites.EffectSuite3()->AEGP_ApplyEffect(myAEGPID, LayerH, key_RENLINE, &EffectRefH);
			suites.EffectSuite3()->AEGP_DisposeEffect(EffectRefH);
			break;
		case 3:
			suites.PFInterfaceSuite1()->AEGP_GetEffectLayer(in_data->effect_ref, &LayerH);
			suites.EffectSuite3()->AEGP_ApplyEffect(myAEGPID, LayerH, key_RENRING, &EffectRefH);
			suites.EffectSuite3()->AEGP_DisposeEffect(EffectRefH);
			
			break;
		default:
			break;
		}
	}
	else if (extra->param_index == CREATENUM_BTN) {
		AEGP_LayerH LayerH;
		AEGP_EffectRefH EffectRefH;
		suites.PFInterfaceSuite1()->AEGP_GetEffectLayer(in_data->effect_ref, &LayerH);
		suites.EffectSuite3()->AEGP_ApplyEffect(myAEGPID, LayerH,key_NUM, &EffectRefH);
		suites.EffectSuite3()->AEGP_DisposeEffect(EffectRefH);
	}
	else if (extra->param_index == CREATELINKER_BTN) {
		AEGP_LayerH LayerH;
		AEGP_EffectRefH EffectRefH;
		suites.PFInterfaceSuite1()->AEGP_GetEffectLayer(in_data->effect_ref, &LayerH);
		suites.EffectSuite3()->AEGP_ApplyEffect(myAEGPID, LayerH, key_LINK, &EffectRefH);
		suites.EffectSuite3()->AEGP_DisposeEffect(EffectRefH);
		suites.ANSICallbacksSuite1()->sprintf(out_data->return_msg, "Because of the rendering priority in After Effects, \nwhen you change a param in the layer which the linker links to,\n it often doesn't call the renderer for changes.\nPlease use with caution.\n(Press the refresh button on the linker to rerender the whole frame.)");
	}
	return PF_Err_NONE;
}


DllExport	
PF_Err 
EntryPointFuncM (
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
			case PF_Cmd_USER_CHANGED_PARAM:
				err = ChangeParam(in_data,
					params,out_data,
					output, (PF_UserChangedParamExtra*)extra);
				break;
			case PF_Cmd_ABOUT:

				err = About(in_data,
							out_data,
							params,
							output);
				break;
				
			case PF_Cmd_GLOBAL_SETUP:

				err = GlobalSetup(	in_data,
									out_data,
									params,
									output);
				break;
				
			case PF_Cmd_PARAMS_SETUP:

				err = ParamsSetup(	in_data,
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
				break;
		}
	}
	catch(PF_Err &thrown_err){
		err = thrown_err;
	}
	return err;
}


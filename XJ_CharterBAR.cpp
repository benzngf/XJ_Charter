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
About_B(
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
static AEGP_PluginID myAEGPID_B;
static AEGP_InstalledEffectKey key_NUM_B = 0, key_MAIN_B = 0, key_LINK_B = 0;
static PF_Err
GlobalSetup_B(
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
	out_data->out_flags = PF_OutFlag_I_AM_OBSOLETE| PF_OutFlag_NON_PARAM_VARY;
	out_data->out_flags2 = PF_OutFlag2_NONE;
	AEGP_SuiteHandler suites(in_data->pica_basicP);
	suites.UtilitySuite5()->AEGP_RegisterWithAEGP(NULL, out_data->name, &myAEGPID_B);
	AEGP_InstalledEffectKey keyTMP = AEGP_InstalledEffectKey_NONE;
	/*FIND THE KEY FOR THE EFFECTS!!!!*/
	A_long Total_effects_count;
	suites.EffectSuite3()->AEGP_GetNumInstalledEffects(&Total_effects_count);
	A_long now = 0;
	A_char matchNameTMP[AEGP_MAX_EFFECT_MATCH_NAME_SIZE + 1];
	A_char name_NUM[AEGP_MAX_EFFECT_MATCH_NAME_SIZE + 1], name_MAIN[AEGP_MAX_EFFECT_MATCH_NAME_SIZE + 1]
		, name_LINK[AEGP_MAX_EFFECT_MATCH_NAME_SIZE + 1];
	PF_STRCPY(name_NUM, "XJ_CharterNUM");
	PF_STRCPY(name_MAIN, "XJ_Charter");
	PF_STRCPY(name_LINK, "XJ_CharterLINK");
	while (now < Total_effects_count) {
		suites.EffectSuite3()->AEGP_GetNextInstalledEffect(keyTMP, &keyTMP);
		suites.EffectSuite3()->AEGP_GetEffectMatchName(keyTMP, matchNameTMP);
		if (!strcmp(name_NUM, matchNameTMP)) {
			key_NUM_B = keyTMP;
		}
		else if (!strcmp(name_MAIN, matchNameTMP)) {
			key_MAIN_B = keyTMP;
		}
		else if (!strcmp(name_LINK, matchNameTMP)) {
			key_LINK_B = keyTMP;
		}
		if ((key_NUM_B != 0) && (key_MAIN_B != 0) && (key_LINK_B!=0)) {
			break;
		}
		now++;
	}

	return PF_Err_NONE;
}

enum PARAMS_B {
	HOR_VER_POPUP = 1,
	START_POINT,
	MAXLEN_FS,
	STANDARD_FS,
	SPACING_FS,
	WIDTH_FS,
	USE_COLOR_CBOX,
	COLOR_C,
	COMPOSIT_POPUP
};

static PF_Err
ParamsSetup_B(
	PF_InData		*in_data,
	PF_OutData		*out_data,
	PF_ParamDef		*params[],
	PF_LayerDef		*output)
{
	PF_Err		err = PF_Err_NONE;
	PF_ParamDef	def;

	AEFX_CLR_STRUCT(def);
	out_data->num_params = 1;

	enum Popup_1 { VERTICAL = 1, HORIZONTAL};
	A_char PopupStr_1[100];
	PF_STRCPY(PopupStr_1, "Vertical|Horizontal");
	def.param_type = PF_Param_POPUP;
	PF_STRCPY(def.name, "Direction");
	def.uu.id = 3;
	def.u.pd.num_choices = 2;
	def.u.pd.dephault = VERTICAL;
	def.u.pd.value = def.u.pd.dephault;
	def.u.pd.u.namesptr = PopupStr_1;
	if (err = PF_ADD_PARAM(in_data, -1, &def))
		return err;
	AEFX_CLR_STRUCT(def);
	out_data->num_params++;

	def.param_type = PF_Param_POINT;
	PF_STRCPY(def.name, "Start Point");
	def.uu.id = 9;
	def.u.td.restrict_bounds = false;
	def.u.td.x_dephault = 50L<<16;
	def.u.td.y_dephault = 50L << 16;
	if (err = PF_ADD_PARAM(in_data, -1, &def))
		return err;
	AEFX_CLR_STRUCT(def);
	out_data->num_params++;

	def.param_type = PF_Param_FLOAT_SLIDER;
	PF_STRCPY(def.name, "Max. Length");
	def.uu.id = 2;
	def.u.fs_d.precision = 0;
	def.u.fs_d.dephault = 360;
	def.u.fs_d.slider_min = 0;
	def.u.fs_d.slider_max = 2000;
	def.u.fs_d.valid_max = 100000;
	def.u.fs_d.valid_min = 0;
	def.u.fs_d.display_flags = 0;
	if (err = PF_ADD_PARAM(in_data, -1, &def))
		return err;
	AEFX_CLR_STRUCT(def);
	out_data->num_params++;

	def.param_type = PF_Param_FLOAT_SLIDER;
	PF_STRCPY(def.name, "Standard Max. Value");
	def.uu.id = 1;
	def.u.fs_d.precision = 1;
	def.u.fs_d.dephault = 100;
	def.u.fs_d.slider_min = 0;
	def.u.fs_d.slider_max = 2000;
	def.u.fs_d.valid_max = 36000000;
	def.u.fs_d.valid_min = 0;
	def.u.fs_d.display_flags = 0;
	if (err = PF_ADD_PARAM(in_data, -1, &def))
		return err;
	AEFX_CLR_STRUCT(def);
	out_data->num_params++;

	def.param_type = PF_Param_FLOAT_SLIDER;
	PF_STRCPY(def.name, "Spacing");
	def.uu.id = 4;
	def.u.fs_d.precision = 0;
	def.u.fs_d.dephault = 80;
	def.u.fs_d.slider_min = 0;
	def.u.fs_d.slider_max = 200;
	def.u.fs_d.valid_max = 36000000;
	def.u.fs_d.valid_min =-36000000;
	def.u.fs_d.display_flags = 0;
	if (err = PF_ADD_PARAM(in_data, -1, &def))
		return err;
	AEFX_CLR_STRUCT(def);
	out_data->num_params++;

	def.param_type = PF_Param_FLOAT_SLIDER;
	PF_STRCPY(def.name, "Bar Width");
	def.uu.id = 5;
	def.u.fs_d.precision = 0;
	def.u.fs_d.dephault = 60;
	def.u.fs_d.slider_min = 0;
	def.u.fs_d.slider_max = 200;
	def.u.fs_d.valid_max = 36000000;
	def.u.fs_d.valid_min = 0;
	def.u.fs_d.display_flags = 0;
	if (err = PF_ADD_PARAM(in_data, -1, &def))
		return err;
	AEFX_CLR_STRUCT(def);
	out_data->num_params++;

	def.param_type = PF_Param_CHECKBOX;
	PF_STRCPY(def.name, "");
	def.uu.id = 6;
	def.u.bd.dephault = true;
	A_char name[10];
	PF_STRCPY(name, "Use Global Color");
	def.u.bd.u.nameptr = name;
	if (err = PF_ADD_PARAM(in_data, -1, &def))
		return err;
	AEFX_CLR_STRUCT(def);
	out_data->num_params++;

	def.param_type = PF_Param_COLOR;
	PF_STRCPY(def.name, "Global Color");
	def.u.cd.dephault.alpha = 255;
	def.u.cd.dephault.red = 0;
	def.u.cd.dephault.blue = 0;
	def.u.cd.dephault.green = 255;
	def.u.cd.value = def.u.cd.dephault;
	def.uu.id = 7;
	def.flags = PF_ParamFlag_NONE;
	if (err = PF_ADD_PARAM(in_data, -1, &def))
		return err;
	AEFX_CLR_STRUCT(def);
	out_data->num_params++;

	A_char PopupStr_2[50];
	PF_STRCPY(PopupStr_2, "On Original Layer|On Transparent");
	def.param_type = PF_Param_POPUP;
	PF_STRCPY(def.name, "Composite mode");
	def.uu.id = 8;
	def.u.pd.num_choices = 2;
	def.u.pd.dephault = 1;
	def.u.pd.value = def.u.pd.dephault;
	def.u.pd.u.namesptr = PopupStr_2;
	if (err = PF_ADD_PARAM(in_data, -1, &def))
		return err;
	AEFX_CLR_STRUCT(def);
	out_data->num_params++;


	return err;
}

static PF_Err
RenderBAR(
	PF_InData		*in_data,
	PF_OutData		*out_data,
	PF_ParamDef		*params[],
	PF_LayerDef		*output)
{
	PF_Err				err = PF_Err_NONE;
	AEGP_SuiteHandler	suites(in_data->pica_basicP);
	if (params[COMPOSIT_POPUP]->u.pd.value == 2L) {
		suites.FillMatteSuite2()->fill(in_data->effect_ref, NULL, NULL, output);
	}else {
		suites.WorldTransformSuite1()->copy_hq(in_data->effect_ref, &params[0]->u.ld, output, NULL, &output->extent_hint);
	}
	
	/*GET index of this effect*/
	AEGP_LayerH LayerH;
	AEGP_EffectRefH myEffectRefH;
	AEGP_StreamRefH myStreamH, myEffStreamH;
	A_long Eff_index = 0;
	suites.PFInterfaceSuite1()->AEGP_GetEffectLayer(in_data->effect_ref, &LayerH);
	suites.PFInterfaceSuite1()->AEGP_GetNewEffectForEffect(myAEGPID_B, in_data->effect_ref, &myEffectRefH);
	suites.StreamSuite4()->AEGP_GetNewEffectStreamByIndex(myAEGPID_B, myEffectRefH, 1, &myStreamH);
	suites.DynamicStreamSuite4()->AEGP_GetNewParentStreamRef(myAEGPID_B, myStreamH, &myEffStreamH);
	suites.DynamicStreamSuite4()->AEGP_GetStreamIndexInParent(myEffStreamH, &Eff_index);
	//suites.ANSICallbacksSuite1()->sprintf(out_data->return_msg,"this effect has index %d", Eff_index);
	suites.StreamSuite4()->AEGP_DisposeStream(myStreamH);
	suites.StreamSuite4()->AEGP_DisposeStream(myEffStreamH);
	suites.EffectSuite3()->AEGP_DisposeEffect(myEffectRefH);

	/* GET ALL THE NUMBERS!!  WHO'S MAX?? GET COLOR TOO?? */
	typedef struct NUM_obj {
		PF_FpLong number;
		NUM_obj *next;
	} NUM_Obj;

	typedef struct NUM_obj_C {
		PF_FpLong number;
		PF_Pixel Color;
		NUM_obj_C *next;
	} NUM_Obj_C;

	AEGP_EffectRefH tmpEffectRefH = NULL;
	NUM_Obj *now = NULL, *head = NULL;
	NUM_Obj_C *now_C = NULL, *head_C = NULL;
	PF_FpLong maxV = 0;
	A_Time current_time;
	suites.PFInterfaceSuite1()->AEGP_ConvertEffectToCompTime(in_data->effect_ref, in_data->current_time, in_data->time_scale, &current_time);
	if (params[USE_COLOR_CBOX]->u.bd.value) {
		A_long index = 0;
		AEGP_InstalledEffectKey ChkKey;
		AEGP_StreamRefH tmpStreamH;
		AEGP_StreamValue2 tmpValue;
		for (index = 0; index < Eff_index; index++) {
			suites.EffectSuite3()->AEGP_GetLayerEffectByIndex(myAEGPID_B, LayerH, index, &tmpEffectRefH);
			suites.EffectSuite3()->AEGP_GetInstalledKeyFromLayerEffect(tmpEffectRefH, &ChkKey);
			if (ChkKey == key_NUM_B) {
				if (now != NULL) {
					now->next = new NUM_Obj;
					now = now->next;
					now->next = NULL;
				}else {
					now = new NUM_Obj;
					head = now;
					now->next = NULL;
				}
				suites.StreamSuite4()->AEGP_GetNewEffectStreamByIndex(myAEGPID_B, tmpEffectRefH, 1, &tmpStreamH);
				suites.StreamSuite4()->AEGP_GetNewStreamValue(myAEGPID_B, tmpStreamH, AEGP_LTimeMode_CompTime,&current_time, false, &tmpValue);
				now->number = tmpValue.val.one_d;
				if(now->number > maxV) maxV = now->number;
				suites.StreamSuite4()->AEGP_DisposeStreamValue(&tmpValue);
				suites.StreamSuite4()->AEGP_DisposeStream(tmpStreamH);
			}
			else if (ChkKey == key_LINK_B) {
				suites.StreamSuite4()->AEGP_GetNewEffectStreamByIndex(myAEGPID_B, tmpEffectRefH, 1, &tmpStreamH);
				suites.StreamSuite4()->AEGP_GetNewStreamValue(myAEGPID_B, tmpStreamH, AEGP_LTimeMode_CompTime, &current_time, false, &tmpValue);
				AEGP_CompH CompH = NULL;
				AEGP_LayerH Link_LayerH = NULL;
				AEGP_EffectRefH Link_tmpEffectRefH = NULL;
				A_long link_eff_index = 0;
				suites.LayerSuite7()->AEGP_GetLayerParentComp(LayerH, &CompH);
				if (tmpValue.val.layer_id != 0) {
					suites.LayerSuite7()->AEGP_GetLayerFromLayerID(CompH, tmpValue.val.layer_id, &Link_LayerH);
					suites.EffectSuite3()->AEGP_GetLayerNumEffects(Link_LayerH, &link_eff_index);
					for (A_long Link_index = 0; Link_index < link_eff_index; Link_index++) {
						suites.EffectSuite3()->AEGP_GetLayerEffectByIndex(myAEGPID_B, Link_LayerH, Link_index, &Link_tmpEffectRefH);
						suites.EffectSuite3()->AEGP_GetInstalledKeyFromLayerEffect(Link_tmpEffectRefH, &ChkKey);
						if (ChkKey == key_NUM_B) {
							if (now != NULL) {
								now->next = new NUM_Obj;
								now = now->next;
								now->next = NULL;
							}
							else {
								now = new NUM_Obj;
								head = now;
								now->next = NULL;
							}
							suites.StreamSuite4()->AEGP_GetNewEffectStreamByIndex(myAEGPID_B, Link_tmpEffectRefH, 1, &tmpStreamH);
							suites.StreamSuite4()->AEGP_GetNewStreamValue(myAEGPID_B, tmpStreamH, AEGP_LTimeMode_CompTime, &current_time, false, &tmpValue);
							now->number = tmpValue.val.one_d;
							if (now->number > maxV) maxV = now->number;
							suites.StreamSuite4()->AEGP_DisposeStreamValue(&tmpValue);
							suites.StreamSuite4()->AEGP_DisposeStream(tmpStreamH);
						}
						suites.EffectSuite3()->AEGP_DisposeEffect(Link_tmpEffectRefH);
					}
				}

			}
			suites.EffectSuite3()->AEGP_DisposeEffect(tmpEffectRefH);
		}
	
	}else {
		A_long index = 0;
		AEGP_InstalledEffectKey ChkKey;
		AEGP_StreamRefH tmpStreamH;
		AEGP_StreamValue2 tmpValue;
		
		for (index = 0; index < Eff_index; index++) {
			suites.EffectSuite3()->AEGP_GetLayerEffectByIndex(myAEGPID_B, LayerH, index, &tmpEffectRefH);
			suites.EffectSuite3()->AEGP_GetInstalledKeyFromLayerEffect(tmpEffectRefH, &ChkKey);
			if (ChkKey == key_NUM_B) {
				if (now_C != NULL) {
					now_C->next = new NUM_Obj_C;
					now_C = now_C->next;
					now_C->next = NULL;
				}
				else {
					now_C = new NUM_Obj_C;
					head_C = now_C;
					now_C->next = NULL;
				}
				suites.StreamSuite4()->AEGP_GetNewEffectStreamByIndex(myAEGPID_B, tmpEffectRefH, 1, &tmpStreamH);
				suites.StreamSuite4()->AEGP_GetNewStreamValue(myAEGPID_B, tmpStreamH, AEGP_LTimeMode_CompTime, &current_time, false, &tmpValue);
				now_C->number = tmpValue.val.one_d;
				if (now_C->number > maxV) maxV = now_C->number;
				suites.StreamSuite4()->AEGP_DisposeStreamValue(&tmpValue);
				suites.StreamSuite4()->AEGP_DisposeStream(tmpStreamH);

				suites.StreamSuite4()->AEGP_GetNewEffectStreamByIndex(myAEGPID_B, tmpEffectRefH, 2, &tmpStreamH);
				suites.StreamSuite4()->AEGP_GetNewStreamValue(myAEGPID_B, tmpStreamH, AEGP_LTimeMode_CompTime, &current_time, false, &tmpValue);
				now_C->Color.alpha = 255;
				now_C->Color.red = (A_u_char)(FLOAT2FIX2((tmpValue.val.color.redF) * 255) >> 16);
				now_C->Color.green = (A_u_char)(FLOAT2FIX2((tmpValue.val.color.greenF) * 255) >> 16);
				now_C->Color.blue = (A_u_char)(FLOAT2FIX2((tmpValue.val.color.blueF) * 255) >> 16);
				//suites.ANSICallbacksSuite1()->sprintf(out_data->return_msg,"Color[%d,%d,%d,%d]", now_C->Color.alpha, now_C->Color.red, now_C->Color.green, now_C->Color.blue);
				suites.StreamSuite4()->AEGP_DisposeStreamValue(&tmpValue);
				suites.StreamSuite4()->AEGP_DisposeStream(tmpStreamH);

			}
			else if (ChkKey == key_LINK_B) {
				suites.StreamSuite4()->AEGP_GetNewEffectStreamByIndex(myAEGPID_B, tmpEffectRefH, 1, &tmpStreamH);
				suites.StreamSuite4()->AEGP_GetNewStreamValue(myAEGPID_B, tmpStreamH, AEGP_LTimeMode_CompTime, &current_time, false, &tmpValue);
				AEGP_CompH CompH = NULL;
				AEGP_LayerH Link_LayerH = NULL;
				AEGP_EffectRefH Link_tmpEffectRefH = NULL;
				A_long link_eff_index = 0;
				suites.LayerSuite7()->AEGP_GetLayerParentComp(LayerH, &CompH);
				if (tmpValue.val.layer_id != 0) {
					suites.LayerSuite7()->AEGP_GetLayerFromLayerID(CompH, tmpValue.val.layer_id, &Link_LayerH);
					suites.EffectSuite3()->AEGP_GetLayerNumEffects(Link_LayerH, &link_eff_index);
					for (A_long Link_index = 0; Link_index < link_eff_index; Link_index++) {
						suites.EffectSuite3()->AEGP_GetLayerEffectByIndex(myAEGPID_B, Link_LayerH, Link_index, &Link_tmpEffectRefH);
						suites.EffectSuite3()->AEGP_GetInstalledKeyFromLayerEffect(Link_tmpEffectRefH, &ChkKey);
						if (ChkKey == key_NUM_B) {
							if (now_C != NULL) {
								now_C->next = new NUM_Obj_C;
								now_C = now_C->next;
								now_C->next = NULL;
							}
							else {
								now_C = new NUM_Obj_C;
								head_C = now_C;
								now_C->next = NULL;
							}
							suites.StreamSuite4()->AEGP_GetNewEffectStreamByIndex(myAEGPID_B, Link_tmpEffectRefH, 1, &tmpStreamH);
							suites.StreamSuite4()->AEGP_GetNewStreamValue(myAEGPID_B, tmpStreamH, AEGP_LTimeMode_CompTime, &current_time, false, &tmpValue);
							now_C->number = tmpValue.val.one_d;
							if (now_C->number > maxV) maxV = now_C->number;
							suites.StreamSuite4()->AEGP_DisposeStreamValue(&tmpValue);
							suites.StreamSuite4()->AEGP_DisposeStream(tmpStreamH);

							suites.StreamSuite4()->AEGP_GetNewEffectStreamByIndex(myAEGPID_B, Link_tmpEffectRefH, 2, &tmpStreamH);
							suites.StreamSuite4()->AEGP_GetNewStreamValue(myAEGPID_B, tmpStreamH, AEGP_LTimeMode_CompTime, &current_time, false, &tmpValue);
							now_C->Color.alpha = 255;
							now_C->Color.red = (A_u_char)(FLOAT2FIX2((tmpValue.val.color.redF) * 255) >> 16);
							now_C->Color.green = (A_u_char)(FLOAT2FIX2((tmpValue.val.color.greenF) * 255) >> 16);
							now_C->Color.blue = (A_u_char)(FLOAT2FIX2((tmpValue.val.color.blueF) * 255) >> 16);
							suites.StreamSuite4()->AEGP_DisposeStreamValue(&tmpValue);
							suites.StreamSuite4()->AEGP_DisposeStream(tmpStreamH);

						}
						suites.EffectSuite3()->AEGP_DisposeEffect(Link_tmpEffectRefH);
					}
				}
			}
				suites.EffectSuite3()->AEGP_DisposeEffect(tmpEffectRefH);
			
		}
	}
	
	/*RENDER THE RECTANGLES*/
	if (maxV != 0 && ((head != NULL)||(head_C!=NULL))) {
		PF_Fixed x = params[START_POINT]->u.td.x_value;
		PF_Fixed y = params[START_POINT]->u.td.y_value;
		PF_Rect destination;
		
		//Dealing width down sampling
		PF_FpLong ScaledWidth, ScaledSpacing, ScaledMaxLen;
		if (params[HOR_VER_POPUP]->u.pd.value == 1) {
			ScaledWidth = (params[WIDTH_FS]->u.fs_d.value)*in_data->downsample_x.num / in_data->downsample_x.den;
			ScaledSpacing = (params[SPACING_FS]->u.fs_d.value)*in_data->downsample_x.num / in_data->downsample_x.den;
			ScaledMaxLen = (params[MAXLEN_FS]->u.fs_d.value)*in_data->downsample_y.num / in_data->downsample_y.den;
		}else {
			ScaledWidth = (params[WIDTH_FS]->u.fs_d.value)*in_data->downsample_y.num / in_data->downsample_y.den;
			ScaledSpacing = (params[SPACING_FS]->u.fs_d.value)*in_data->downsample_y.num / in_data->downsample_y.den;
			ScaledMaxLen = (params[MAXLEN_FS]->u.fs_d.value)*in_data->downsample_x.num / in_data->downsample_x.den;
		}

		maxV = (maxV > params[STANDARD_FS]->u.fs_d.value)? maxV: params[STANDARD_FS]->u.fs_d.value;
		if (params[USE_COLOR_CBOX]->u.bd.value) {
			if (params[HOR_VER_POPUP]->u.pd.value == 1) {
				destination.bottom = (y) >> 16;
				for (now = head, head = NULL; now != NULL; now = now->next, x += FLOAT2FIX2(ScaledSpacing)) {
					delete head;
					destination.left = (x - FLOAT2FIX2(ScaledWidth / 2)) >> 16;
					destination.right = (x + FLOAT2FIX2(ScaledWidth / 2)) >> 16;
					destination.top = (y - FLOAT2FIX2((now->number / maxV)*ScaledMaxLen)) >> 16;
					suites.FillMatteSuite2()->fill(in_data->effect_ref, &params[COLOR_C]->u.cd.value, &destination, output);
					//suites.ANSICallbacksSuite1()->sprintf(out_data->return_msg,"Create Rectangle [%d,%d,%d,%d]", destination.bottom, destination.top, destination.left, destination.right);
					head = now;
				}
			}
			else {
				destination.left = (x) >> 16;
				for (now = head, head = NULL; now != NULL; now = now->next, y += FLOAT2FIX2(ScaledSpacing)) {
					delete head;
					destination.top = (y - FLOAT2FIX2(ScaledWidth / 2)) >> 16;
					destination.bottom = (y + FLOAT2FIX2(ScaledWidth / 2)) >> 16;
					destination.right = (x + FLOAT2FIX2((now->number / maxV)*ScaledMaxLen)) >> 16;
					suites.FillMatteSuite2()->fill(in_data->effect_ref, &params[COLOR_C]->u.cd.value, &destination, output);
					//suites.ANSICallbacksSuite1()->sprintf(out_data->return_msg,"Create Rectangle [%d,%d,%d,%d]", destination.bottom, destination.top, destination.left, destination.right);
					head = now;
				}
			}
		}else {
			if (params[HOR_VER_POPUP]->u.pd.value == 1) {
				destination.bottom = (y) >> 16;
				for (now_C = head_C, head_C = NULL; now_C != NULL; now_C = now_C->next, x += FLOAT2FIX2(ScaledSpacing)) {
					delete head_C;
					destination.left = (x - FLOAT2FIX2(ScaledWidth / 2)) >> 16;
					destination.right = (x + FLOAT2FIX2(ScaledWidth / 2)) >> 16;
					destination.top = (y - FLOAT2FIX2((now_C->number / maxV)*ScaledMaxLen)) >> 16;
					suites.FillMatteSuite2()->fill(in_data->effect_ref, &now_C->Color, &destination, output);
					//suites.ANSICallbacksSuite1()->sprintf(out_data->return_msg,"Create Rectangle [%d,%d,%d,%d]", destination.bottom, destination.top, destination.left, destination.right);
					head_C = now_C;
				}
			}else {
				destination.left = (x) >> 16;
				for (now_C = head_C, head_C = NULL; now_C != NULL; now_C = now_C->next, y += FLOAT2FIX2(ScaledSpacing)) {
					delete head_C;
					destination.top = (y - FLOAT2FIX2(ScaledWidth / 2)) >> 16;
					destination.bottom = (y + FLOAT2FIX2(ScaledWidth / 2)) >> 16;
					destination.right = (x + FLOAT2FIX2((now_C->number / maxV)*ScaledMaxLen)) >> 16;
					suites.FillMatteSuite2()->fill(in_data->effect_ref, &(now_C->Color), &destination, output);
					//suites.ANSICallbacksSuite1()->sprintf(out_data->return_msg,"Create Rectangle [%d,%d,%d,%d]", destination.bottom, destination.top, destination.left, destination.right);
					head_C = now_C;
				}
			}
		}
	}
	return err;
}

DllExport
PF_Err
EntryPointFuncB(
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

			err = About_B(in_data,
				out_data,
				params,
				output);
			break;

		case PF_Cmd_GLOBAL_SETUP:

			err = GlobalSetup_B(in_data,
				out_data,
				params,
				output);
			break;

		case PF_Cmd_PARAMS_SETUP:

			err = ParamsSetup_B(in_data,
				out_data,
				params,
				output);
			break;

		/*case PF_Cmd_FRAME_SETUP:
			out_data->out_flags |= PF_OutFlag_NOP_RENDER;
			err = PF_Err_NONE;
			break;*/
		case PF_Cmd_RENDER:
			err = RenderBAR(in_data,
				out_data,
				params,
				output);
			break;
		}
	}
	catch (PF_Err &thrown_err) {
		err = thrown_err;
	}
	return err;
}


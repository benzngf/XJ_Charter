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
About_R(
	PF_InData		*in_data,
	PF_OutData		*out_data,
	PF_ParamDef		*params[],
	PF_LayerDef		*output)
{
	AEGP_SuiteHandler suites(in_data->pica_basicP);

	suites.ANSICallbacksSuite1()->sprintf(out_data->return_msg,
		"大家安安大家好OUO/ \n\n有任何BUG、問題\n或想新加的功能\n請聯絡小捲\n  benzngf@gmail.com  \n感謝您～");
	return PF_Err_NONE;
}
static AEGP_PluginID myAEGPID_R;
static AEGP_InstalledEffectKey key_NUM_R = 0, key_MAIN_R = 0, key_LINK_R = 0;
static PF_Err
GlobalSetup_R(
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
	out_data->out_flags = PF_OutFlag_I_AM_OBSOLETE | PF_OutFlag_NON_PARAM_VARY;
	out_data->out_flags2 = PF_OutFlag2_NONE;
	AEGP_SuiteHandler suites(in_data->pica_basicP);
	suites.UtilitySuite5()->AEGP_RegisterWithAEGP(NULL, out_data->name, &myAEGPID_R);
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
			key_NUM_R = keyTMP;
		}
		else if (!strcmp(name_MAIN, matchNameTMP)) {
			key_MAIN_R = keyTMP;
		}
		else if (!strcmp(name_LINK, matchNameTMP)) {
			key_LINK_R = keyTMP;
		}
		if ((key_NUM_R != 0) && (key_MAIN_R != 0) && (key_LINK_R != 0)) {
			break;
		}
		now++;
	}

	return PF_Err_NONE;
}

enum PARAMS_R {
	FILL_STROKE_POPUP = 1,
	START_POINT,
	ROTATION_ANG,
	IN_RADIUS_FS,
	OUT_RADIUS_FS,
	FULL_PERCENT_FS,

	FILL_GROUPSTART,

	GLOBAL_COLOR_CHKBOX,
	FILL_COLOR_C,
	PHASE_POPUP,
	PLUS_MINUS_POPUP,
	PHASE_OFFSET_FS,

	FILL_GROUPEND,
	
	STROKE_GROUPSTART,
	
	WIDTH_FS,
	SPACING_FS,
	FEATHER_FS,
	STROKE_COLOR_C,
	//STROKE_OFFSET_POINT,

	STROKE_GROUPEND,
	
	COMPOSIT_POPUP
};

static PF_Err
ParamsSetup_R(
	PF_InData		*in_data,
	PF_OutData		*out_data,
	PF_ParamDef		*params[],
	PF_LayerDef		*output)
{
	PF_Err		err = PF_Err_NONE;
	PF_ParamDef	def;

	AEFX_CLR_STRUCT(def);
	out_data->num_params = 1;

	enum Popup_1 { FILL = 1, STROKE, FILL_STROKE };
	A_char PopupStr_1[100];
	PF_STRCPY(PopupStr_1, "Fill Only|Stroke Only|Stroke & Fill");
	def.param_type = PF_Param_POPUP;
	PF_STRCPY(def.name, "Fill/Stroke Mode");
	def.uu.id = 1;
	def.u.pd.num_choices = 3;
	def.u.pd.dephault = FILL_STROKE;
	def.u.pd.value = def.u.pd.dephault;
	def.u.pd.u.namesptr = PopupStr_1;
	if (err = PF_ADD_PARAM(in_data, -1, &def))
		return err;
	AEFX_CLR_STRUCT(def);
	out_data->num_params++;

	def.param_type = PF_Param_POINT;
	PF_STRCPY(def.name, "Start Point");
	def.uu.id = 2;
	def.u.td.restrict_bounds = false;
	def.u.td.x_dephault = 50L << 16;
	def.u.td.y_dephault = 50L << 16;
	if (err = PF_ADD_PARAM(in_data, -1, &def))
		return err;
	AEFX_CLR_STRUCT(def);
	out_data->num_params++;

	def.param_type = PF_Param_ANGLE;
	PF_STRCPY(def.name, "Rotation");
	def.uu.id = 28;
	def.u.ad.dephault = 0;
	def.u.ad.valid_max = 360000 << 16;
	def.u.ad.valid_min = -360000 << 16;
	if (err = PF_ADD_PARAM(in_data, -1, &def))
		return err;
	AEFX_CLR_STRUCT(def);
	out_data->num_params++;

	def.param_type = PF_Param_FLOAT_SLIDER;
	PF_STRCPY(def.name, "Inner Radius");
	def.uu.id = 3;
	def.u.fs_d.precision = 1;
	def.u.fs_d.dephault = 0;
	def.u.fs_d.slider_min = 0;
	def.u.fs_d.slider_max = 250;
	def.u.fs_d.valid_max = 100000;
	def.u.fs_d.valid_min = 0;
	def.u.fs_d.display_flags = 0;
	if (err = PF_ADD_PARAM(in_data, -1, &def))
		return err;
	AEFX_CLR_STRUCT(def);
	out_data->num_params++;

	def.param_type = PF_Param_FLOAT_SLIDER;
	PF_STRCPY(def.name, "Outer Radius");
	def.uu.id = 4;
	def.u.fs_d.precision = 1;
	def.u.fs_d.dephault = 150;
	def.u.fs_d.slider_min = 0;
	def.u.fs_d.slider_max = 250;
	def.u.fs_d.valid_max = 100000;
	def.u.fs_d.valid_min = 0;
	def.u.fs_d.display_flags = 0;
	if (err = PF_ADD_PARAM(in_data, -1, &def))
		return err;
	AEFX_CLR_STRUCT(def);
	out_data->num_params++;

	def.param_type = PF_Param_FLOAT_SLIDER;
	PF_STRCPY(def.name, "Complete Percentage");
	def.uu.id = 5;
	def.u.fs_d.dephault = 100;
	def.u.fs_d.slider_min = 0;
	def.u.fs_d.slider_max = 100;
	def.u.fs_d.valid_max = 100;
	def.u.fs_d.valid_min = 0;
	def.u.fs_d.precision = 0;
	def.u.fs_d.display_flags = 1;
	if (err = PF_ADD_PARAM(in_data, -1, &def))
		return err;
	AEFX_CLR_STRUCT(def);
	out_data->num_params++;

	def.param_type = PF_Param_GROUP_START;
	PF_STRCPY(def.name, "Fill");
	def.uu.id = 6;
	def.flags = PF_ParamFlag_START_COLLAPSED;
	if (err = PF_ADD_PARAM(in_data, -1, &def))
		return err;
	AEFX_CLR_STRUCT(def);
	out_data->num_params++;

	def.param_type = PF_Param_CHECKBOX;
	PF_STRCPY(def.name, "");
	def.uu.id = 7;
	def.u.bd.dephault = true;
	A_char name[10];
	PF_STRCPY(name, "Use Global Color&Variation");
	def.u.bd.u.nameptr = name;
	if (err = PF_ADD_PARAM(in_data, -1, &def))
		return err;
	AEFX_CLR_STRUCT(def);
	out_data->num_params++;

	def.param_type = PF_Param_COLOR;
	PF_STRCPY(def.name, "Global Color");
	def.u.cd.dephault.alpha = 255;
	def.u.cd.dephault.red = 150;
	def.u.cd.dephault.blue = 150;
	def.u.cd.dephault.green = 255;
	def.u.cd.value = def.u.cd.dephault;
	def.uu.id = 9;
	def.flags = PF_ParamFlag_NONE;
	if (err = PF_ADD_PARAM(in_data, -1, &def))
		return err;
	AEFX_CLR_STRUCT(def);
	out_data->num_params++;

	A_char PopupStr_3[30];
	PF_STRCPY(PopupStr_3, "Hue|Lightness|Saturation");
	def.param_type = PF_Param_POPUP;
	PF_STRCPY(def.name, "Change by");
	def.uu.id = 8;
	def.u.pd.num_choices = 3;
	def.u.pd.dephault = 1;
	def.u.pd.value = def.u.pd.dephault;
	def.u.pd.u.namesptr = PopupStr_3;
	if (err = PF_ADD_PARAM(in_data, -1, &def))
		return err;
	AEFX_CLR_STRUCT(def);
	out_data->num_params++;

	A_char PopupStr_4[30];
	PF_STRCPY(PopupStr_4, "Increase|Decrease");
	def.param_type = PF_Param_POPUP;
	PF_STRCPY(def.name, "Gradually Increase/Decrease");
	def.uu.id = 38;
	def.u.pd.num_choices = 2;
	def.u.pd.dephault = 1;
	def.u.pd.value = def.u.pd.dephault;
	def.u.pd.u.namesptr = PopupStr_4;
	if (err = PF_ADD_PARAM(in_data, -1, &def))
		return err;
	AEFX_CLR_STRUCT(def);
	out_data->num_params++;

	def.param_type = PF_Param_FLOAT_SLIDER;
	PF_STRCPY(def.name, "Color Variation");
	def.uu.id = 10;
	def.u.fs_d.dephault = 100;
	def.u.fs_d.slider_min = 0;
	def.u.fs_d.slider_max = 100;
	def.u.fs_d.valid_max = 100;
	def.u.fs_d.valid_min = 0;
	def.u.fs_d.precision = 0;
	def.u.fs_d.display_flags = 1;
	if (err = PF_ADD_PARAM(in_data, -1, &def))
		return err;
	AEFX_CLR_STRUCT(def);
	out_data->num_params++;

	def.param_type = PF_Param_GROUP_END;
	def.uu.id = 11;
	if (err = PF_ADD_PARAM(in_data, -1, &def))
		return err;
	AEFX_CLR_STRUCT(def);
	out_data->num_params++;

	def.param_type = PF_Param_GROUP_START;
	PF_STRCPY(def.name, "Stroke");
	def.uu.id = 12;
	def.flags = PF_ParamFlag_START_COLLAPSED;
	if (err = PF_ADD_PARAM(in_data, -1, &def))
		return err;
	AEFX_CLR_STRUCT(def);
	out_data->num_params++;

	def.param_type = PF_Param_FLOAT_SLIDER;
	PF_STRCPY(def.name, "Stroke Width");
	def.uu.id = 13;
	def.u.fs_d.precision = 1;
	def.u.fs_d.dephault = 5;
	def.u.fs_d.slider_min = 0;
	def.u.fs_d.slider_max = 50;
	def.u.fs_d.valid_max = 36000000;
	def.u.fs_d.valid_min = 0;
	def.u.fs_d.display_flags = 0;
	if (err = PF_ADD_PARAM(in_data, -1, &def))
		return err;
	AEFX_CLR_STRUCT(def);
	out_data->num_params++;

	def.param_type = PF_Param_FLOAT_SLIDER;
	PF_STRCPY(def.name, "Stroke Spacing");
	def.uu.id = 14;
	def.u.fs_d.precision = 1;
	def.u.fs_d.dephault = 1;
	def.u.fs_d.slider_min = (PF_FpShort)0.1;
	def.u.fs_d.slider_max = 50;
	def.u.fs_d.valid_max = 100;
	def.u.fs_d.valid_min = (PF_FpShort)0.1;
	def.u.fs_d.display_flags = 0;
	if (err = PF_ADD_PARAM(in_data, -1, &def))
		return err;
	AEFX_CLR_STRUCT(def);
	out_data->num_params++;

	def.param_type = PF_Param_FLOAT_SLIDER;
	PF_STRCPY(def.name, "Stroke Feather");
	def.uu.id = 32;
	def.u.fs_d.dephault = 100;
	def.u.fs_d.slider_min = 0;
	def.u.fs_d.slider_max = 100;
	def.u.fs_d.valid_max = 100;
	def.u.fs_d.valid_min = 0;
	def.u.fs_d.precision = 0;
	def.u.fs_d.display_flags = 1;
	if (err = PF_ADD_PARAM(in_data, -1, &def))
		return err;
	AEFX_CLR_STRUCT(def);
	out_data->num_params++;

	def.param_type = PF_Param_COLOR;
	PF_STRCPY(def.name, "Stroke Color");
	def.u.cd.dephault.alpha = 255;
	def.u.cd.dephault.red = 230;
	def.u.cd.dephault.blue = 230;
	def.u.cd.dephault.green = 230;
	def.u.cd.value = def.u.cd.dephault;
	def.uu.id = 15;
	def.flags = PF_ParamFlag_NONE;
	if (err = PF_ADD_PARAM(in_data, -1, &def))
		return err;
	AEFX_CLR_STRUCT(def);
	out_data->num_params++;
	

	def.param_type = PF_Param_GROUP_END;
	def.uu.id = 24;
	if (err = PF_ADD_PARAM(in_data, -1, &def))
		return err;
	AEFX_CLR_STRUCT(def);
	out_data->num_params++;

	A_char PopupStr_2[50];
	PF_STRCPY(PopupStr_2, "On Original Layer|On Transparent");
	def.param_type = PF_Param_POPUP;
	PF_STRCPY(def.name, "Composite Mode");
	def.uu.id = 25;
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

/*Functions needed in render*/
typedef struct color_feather {
	PF_Pixel color;
	A_long Radius;
	A_long Start_Feather_Radius;
} Color_Feather;
PF_Err Make_Circle_Pix(void *refcon, A_long x, A_long y, PF_Pixel *in, PF_Pixel *out) {
	Color_Feather *C_F = (Color_Feather*)refcon;
	x -= C_F->Radius+1;
	y -= C_F->Radius+1;
	if ((C_F->Radius)*(C_F->Radius) < (x*x + y*y)) {
		out->alpha = 0;
	}
	else {
		/*Feather Calc*/
		out->red = C_F->color.red;
		out->green = C_F->color.green;
		out->blue = C_F->color.blue;
		if (C_F->Start_Feather_Radius == C_F->Radius) {
			out->alpha = 255;
			return A_Err_NONE;
		}else if ((C_F->Start_Feather_Radius)*(C_F->Start_Feather_Radius) > (x*x + y*y)) {
			out->alpha = 255;
		}
		else {
			out->alpha = (A_u_char)(255 - (255 * (x*x + y*y - C_F->Start_Feather_Radius*C_F->Start_Feather_Radius) / ((C_F->Radius)*(C_F->Radius) - C_F->Start_Feather_Radius*C_F->Start_Feather_Radius)));
		}
	}
	return A_Err_NONE;
}

typedef struct color_angle {
	PF_Pixel *color;
	A_long in_Radius, out_Radius;
	A_long HalfWorldSize;
	A_FpLong angle;/*in RAD*/
	AEGP_SuiteHandler* suites;
} Color_Angle;

PF_Err Make_Num_Part_Pix1(void *refcon, A_long x, A_long y, PF_Pixel *in, PF_Pixel *out) {
	Color_Angle *C_A = (Color_Angle*)refcon;
	x -= (C_A->HalfWorldSize) - (C_A->out_Radius);
	y -= (C_A->HalfWorldSize) - (C_A->out_Radius);
	A_FpLong Angle = C_A->suites->ANSICallbacksSuite1()->atan2(y, x);
	if (Angle < 0) Angle += PF_2PI;
	if ((C_A->out_Radius)*(C_A->out_Radius) < (x*x + y*y) || (C_A->in_Radius)*(C_A->in_Radius) > (x*x + y*y) || Angle  > C_A->angle) {
		out->red = C_A->color->red;
		out->green = C_A->color->green;
		out->blue = C_A->color->blue;
		out->alpha = 0;
	}else{
		out->red = C_A->color->red;
		out->green = C_A->color->green;
		out->blue = C_A->color->blue;
		out->alpha = 255;
	}
	return A_Err_NONE;
}
PF_Err Make_Num_Part_Pix2(void *refcon, A_long x, A_long y, PF_Pixel *in, PF_Pixel *out) {
	Color_Angle *C_A = (Color_Angle*)refcon;
	x -= C_A->HalfWorldSize;
	y -= (C_A->HalfWorldSize)-(C_A->out_Radius);
	A_FpLong Angle = C_A->suites->ANSICallbacksSuite1()->atan2(y, x);
	if (Angle < 0) Angle += PF_2PI;
	if ((C_A->out_Radius)*(C_A->out_Radius) < (x*x + y*y) || (C_A->in_Radius)*(C_A->in_Radius) > (x*x + y*y) || Angle  > C_A->angle) {
		out->red = C_A->color->red;
		out->green = C_A->color->green;
		out->blue = C_A->color->blue;
		out->alpha =0;
	}
	else {
		out->red = C_A->color->red;
		out->green = C_A->color->green;
		out->blue = C_A->color->blue;
		out->alpha = 255;
	}
	return A_Err_NONE;
}
PF_Err Make_Num_Part_Pix4(void *refcon, A_long x, A_long y, PF_Pixel *in, PF_Pixel *out) {
	Color_Angle *C_A = (Color_Angle*)refcon;
	x -= C_A->HalfWorldSize;
	y -= C_A->HalfWorldSize;
	A_FpLong Angle = C_A->suites->ANSICallbacksSuite1()->atan2(y, x);
	if (Angle < 0) Angle += PF_2PI;
	if ((C_A->out_Radius)*(C_A->out_Radius) < (x*x + y*y) || (C_A->in_Radius)*(C_A->in_Radius) > (x*x + y*y) || Angle  > C_A->angle) {
		out->alpha = 0;
	}
	else {
		out->red = C_A->color->red;
		out->green = C_A->color->green;
		out->blue = C_A->color->blue;
		out->alpha = 255;
	}
	return A_Err_NONE;
}


static PF_Err
RenderRING(
	PF_InData		*in_data,
	PF_OutData		*out_data,
	PF_ParamDef		*params[],
	PF_LayerDef		*output)
{
	PF_Err				err = PF_Err_NONE;
	AEGP_SuiteHandler	suites(in_data->pica_basicP);
	if (params[COMPOSIT_POPUP]->u.pd.value == 2L) {
		suites.FillMatteSuite2()->fill(in_data->effect_ref, NULL, NULL, output);
	}
	else {
		suites.WorldTransformSuite1()->copy_hq(in_data->effect_ref, &params[0]->u.ld, output, NULL, &output->extent_hint);
	}

	/*GET index of this effect*/
	AEGP_LayerH LayerH;
	AEGP_EffectRefH myEffectRefH;
	AEGP_StreamRefH myStreamH, myEffStreamH;
	A_long Eff_index = 0;
	suites.PFInterfaceSuite1()->AEGP_GetEffectLayer(in_data->effect_ref, &LayerH);
	suites.PFInterfaceSuite1()->AEGP_GetNewEffectForEffect(myAEGPID_R, in_data->effect_ref, &myEffectRefH);
	suites.StreamSuite4()->AEGP_GetNewEffectStreamByIndex(myAEGPID_R, myEffectRefH, 1, &myStreamH);
	suites.DynamicStreamSuite4()->AEGP_GetNewParentStreamRef(myAEGPID_R, myStreamH, &myEffStreamH);
	suites.DynamicStreamSuite4()->AEGP_GetStreamIndexInParent(myEffStreamH, &Eff_index);
	suites.StreamSuite4()->AEGP_DisposeStream(myStreamH);
	suites.StreamSuite4()->AEGP_DisposeStream(myEffStreamH);
	suites.EffectSuite3()->AEGP_DisposeEffect(myEffectRefH);

	/* GET ALL THE NUMBERS!!  SUM?? GET COLOR TOO?? */
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
	PF_FpLong sumV = 0;
	A_long total_num = 0;
	A_Time current_time;
	suites.PFInterfaceSuite1()->AEGP_ConvertEffectToCompTime(in_data->effect_ref, in_data->current_time, in_data->time_scale, &current_time);
	if (params[GLOBAL_COLOR_CHKBOX]->u.bd.value) {	//Not Getting Colors
		A_long index = 0;
		AEGP_InstalledEffectKey ChkKey;
		AEGP_StreamRefH tmpStreamH;
		AEGP_StreamValue2 tmpValue;
		for (index = 0; index < Eff_index; index++) {
			suites.EffectSuite3()->AEGP_GetLayerEffectByIndex(myAEGPID_R, LayerH, index, &tmpEffectRefH);
			suites.EffectSuite3()->AEGP_GetInstalledKeyFromLayerEffect(tmpEffectRefH, &ChkKey);
			if (ChkKey == key_NUM_R) {
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
				suites.StreamSuite4()->AEGP_GetNewEffectStreamByIndex(myAEGPID_R, tmpEffectRefH, 1, &tmpStreamH);
				suites.StreamSuite4()->AEGP_GetNewStreamValue(myAEGPID_R, tmpStreamH, AEGP_LTimeMode_CompTime, &current_time, false, &tmpValue);
				now->number = tmpValue.val.one_d;
				sumV += now->number;
				total_num++;
				suites.StreamSuite4()->AEGP_DisposeStreamValue(&tmpValue);
				suites.StreamSuite4()->AEGP_DisposeStream(tmpStreamH);
			}
			else if (ChkKey == key_LINK_R) {
				suites.StreamSuite4()->AEGP_GetNewEffectStreamByIndex(myAEGPID_R, tmpEffectRefH, 1, &tmpStreamH);
				suites.StreamSuite4()->AEGP_GetNewStreamValue(myAEGPID_R, tmpStreamH, AEGP_LTimeMode_CompTime, &current_time, false, &tmpValue);
				AEGP_CompH CompH = NULL;
				AEGP_LayerH Link_LayerH = NULL;
				AEGP_EffectRefH Link_tmpEffectRefH = NULL;
				A_long link_eff_index = 0;
				suites.LayerSuite7()->AEGP_GetLayerParentComp(LayerH, &CompH);
				if (tmpValue.val.layer_id != 0) {
					suites.LayerSuite7()->AEGP_GetLayerFromLayerID(CompH, tmpValue.val.layer_id, &Link_LayerH);
					suites.EffectSuite3()->AEGP_GetLayerNumEffects(Link_LayerH, &link_eff_index);
					for (A_long Link_index = 0; Link_index < link_eff_index; Link_index++) {
						suites.EffectSuite3()->AEGP_GetLayerEffectByIndex(myAEGPID_R, Link_LayerH, Link_index, &Link_tmpEffectRefH);
						suites.EffectSuite3()->AEGP_GetInstalledKeyFromLayerEffect(Link_tmpEffectRefH, &ChkKey);
						if (ChkKey == key_NUM_R) {
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
							suites.StreamSuite4()->AEGP_GetNewEffectStreamByIndex(myAEGPID_R, Link_tmpEffectRefH, 1, &tmpStreamH);
							suites.StreamSuite4()->AEGP_GetNewStreamValue(myAEGPID_R, tmpStreamH, AEGP_LTimeMode_CompTime, &current_time, false, &tmpValue);
							now->number = tmpValue.val.one_d;
							sumV += now->number;
							total_num++;
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
	else {	//Getting Colors
		A_long index = 0;
		AEGP_InstalledEffectKey ChkKey;
		AEGP_StreamRefH tmpStreamH;
		AEGP_StreamValue2 tmpValue;

		for (index = 0; index < Eff_index; index++) {
			suites.EffectSuite3()->AEGP_GetLayerEffectByIndex(myAEGPID_R, LayerH, index, &tmpEffectRefH);
			suites.EffectSuite3()->AEGP_GetInstalledKeyFromLayerEffect(tmpEffectRefH, &ChkKey);
			if (ChkKey == key_NUM_R) {
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
				suites.StreamSuite4()->AEGP_GetNewEffectStreamByIndex(myAEGPID_R, tmpEffectRefH, 1, &tmpStreamH);
				suites.StreamSuite4()->AEGP_GetNewStreamValue(myAEGPID_R, tmpStreamH, AEGP_LTimeMode_CompTime, &current_time, false, &tmpValue);
				now_C->number = tmpValue.val.one_d;
				sumV += now_C->number;
				suites.StreamSuite4()->AEGP_DisposeStreamValue(&tmpValue);
				suites.StreamSuite4()->AEGP_DisposeStream(tmpStreamH);

				suites.StreamSuite4()->AEGP_GetNewEffectStreamByIndex(myAEGPID_R, tmpEffectRefH, 2, &tmpStreamH);
				suites.StreamSuite4()->AEGP_GetNewStreamValue(myAEGPID_R, tmpStreamH, AEGP_LTimeMode_CompTime, &current_time, false, &tmpValue);
				now_C->Color.alpha = 255;
				now_C->Color.red = (A_u_char)(FLOAT2FIX2((tmpValue.val.color.redF) * 255) >> 16);
				now_C->Color.green = (A_u_char)(FLOAT2FIX2((tmpValue.val.color.greenF) * 255) >> 16);
				now_C->Color.blue = (A_u_char)(FLOAT2FIX2((tmpValue.val.color.blueF) * 255) >> 16);
				//suites.ANSICallbacksSuite1()->sprintf(out_data->return_msg,"Color[%d,%d,%d,%d]", now_C->Color.alpha, now_C->Color.red, now_C->Color.green, now_C->Color.blue);
				suites.StreamSuite4()->AEGP_DisposeStreamValue(&tmpValue);
				suites.StreamSuite4()->AEGP_DisposeStream(tmpStreamH);

			}
			else if (ChkKey == key_LINK_R) {
				suites.StreamSuite4()->AEGP_GetNewEffectStreamByIndex(myAEGPID_R, tmpEffectRefH, 1, &tmpStreamH);
				suites.StreamSuite4()->AEGP_GetNewStreamValue(myAEGPID_R, tmpStreamH, AEGP_LTimeMode_CompTime, &current_time, false, &tmpValue);
				AEGP_CompH CompH = NULL;
				AEGP_LayerH Link_LayerH = NULL;
				AEGP_EffectRefH Link_tmpEffectRefH = NULL;
				A_long link_eff_index = 0;
				suites.LayerSuite7()->AEGP_GetLayerParentComp(LayerH, &CompH);
				if (tmpValue.val.layer_id != 0) {
					suites.LayerSuite7()->AEGP_GetLayerFromLayerID(CompH, tmpValue.val.layer_id, &Link_LayerH);
					suites.EffectSuite3()->AEGP_GetLayerNumEffects(Link_LayerH, &link_eff_index);
					for (A_long Link_index = 0; Link_index < link_eff_index; Link_index++) {
						suites.EffectSuite3()->AEGP_GetLayerEffectByIndex(myAEGPID_R, Link_LayerH, Link_index, &Link_tmpEffectRefH);
						suites.EffectSuite3()->AEGP_GetInstalledKeyFromLayerEffect(Link_tmpEffectRefH, &ChkKey);
						if (ChkKey == key_NUM_R) {
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
							suites.StreamSuite4()->AEGP_GetNewEffectStreamByIndex(myAEGPID_R, Link_tmpEffectRefH, 1, &tmpStreamH);
							suites.StreamSuite4()->AEGP_GetNewStreamValue(myAEGPID_R, tmpStreamH, AEGP_LTimeMode_CompTime, &current_time, false, &tmpValue);
							now_C->number = tmpValue.val.one_d;
							sumV += now_C->number;
							suites.StreamSuite4()->AEGP_DisposeStreamValue(&tmpValue);
							suites.StreamSuite4()->AEGP_DisposeStream(tmpStreamH);

							suites.StreamSuite4()->AEGP_GetNewEffectStreamByIndex(myAEGPID_R, Link_tmpEffectRefH, 2, &tmpStreamH);
							suites.StreamSuite4()->AEGP_GetNewStreamValue(myAEGPID_R, tmpStreamH, AEGP_LTimeMode_CompTime, &current_time, false, &tmpValue);
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

	PF_CompositeMode Normal_CmpMode = { PF_Xfer_IN_FRONT , 0, 255, false, 0 };

	if (sumV == 0) {

		if (head != NULL) {
			for (now = head, head = NULL; now != NULL; now = now->next) {
				delete head;
				head = now;
			}
		}
		else if (head_C != NULL) {
			for (now_C = head_C, head_C = NULL; now_C != NULL; now_C = now_C->next) {
				delete head_C;
				head_C = now_C;
			}
		}

		return err;
	}

	//Make circle for stroke if necessary
	PF_EffectWorld stroke_element, line;
	PF_Boolean has_line = false;
	if ((params[FILL_STROKE_POPUP]->u.pd.value == 2 || params[FILL_STROKE_POPUP]->u.pd.value == 3) && (params[WIDTH_FS]->u.fs_d.value != 0)) {
		Color_Feather new_CF;
		new_CF.Radius = (FLOAT2FIX2(params[WIDTH_FS]->u.fs_d.value / 2) >> 16) ;
		new_CF.Start_Feather_Radius = ((FLOAT2FIX2(params[WIDTH_FS]->u.fs_d.value - (params[WIDTH_FS]->u.fs_d.value * params[FEATHER_FS]->u.fs_d.value / (PF_FpLong)100))) >> 16) / 2;
		new_CF.color = params[STROKE_COLOR_C]->u.cd.value;
		suites.WorldSuite1()->new_world(in_data->effect_ref, new_CF.Radius * 2 + 2, new_CF.Radius * 2 + 2, PF_NewWorldFlag_NONE, &stroke_element);
		suites.Iterate8Suite1()->iterate(in_data, 0, 0, NULL, NULL, &new_CF, &Make_Circle_Pix, &stroke_element);
	//Make a line with the circles
		if (params[OUT_RADIUS_FS]->u.fs_d.value - params[IN_RADIUS_FS]->u.fs_d.value > 0) {
			suites.WorldSuite1()->new_world(in_data->effect_ref,
				(FLOAT2FIX2(params[OUT_RADIUS_FS]->u.fs_d.value - params[IN_RADIUS_FS]->u.fs_d.value)>>16)+ stroke_element.width, stroke_element.height, PF_NewWorldFlag_CLEAR_PIXELS, &line);
			PF_FloatMatrix transFormMatrix_Point;
			CCU_SetIdentityMatrix(&transFormMatrix_Point);
			
			PF_FpLong moveDist;
			moveDist = params[SPACING_FS]->u.fs_d.value;
			for (PF_FpLong now = 0; now < (params[OUT_RADIUS_FS]->u.fs_d.value - params[IN_RADIUS_FS]->u.fs_d.value); now += moveDist) {
				suites.WorldTransformSuite1()->transform_world(in_data->effect_ref, in_data->quality, PF_MF_Alpha_PREMUL, PF_Field_FRAME, &stroke_element,
					&Normal_CmpMode, NULL, &transFormMatrix_Point, 1, true, &line.extent_hint, &line);
				CCU_TransformPointsFloat(&transFormMatrix_Point, moveDist, 0);
			}
			has_line = true;
		}
		
	}
	/*CREATED*/
	



	/*Make a 扇形 PF_EffectWorld, optimize size according to angle(0~90, 90~180, 180~360) */
	PF_EffectWorld number_element1, number_element2, number_element4, Tmp_dst;
	Color_Angle new_CA;
	new_CA.out_Radius = (FLOAT2FIX2(params[OUT_RADIUS_FS]->u.fs_d.value) >> 16);
	new_CA.in_Radius = (FLOAT2FIX2(params[IN_RADIUS_FS]->u.fs_d.value) >> 16);
	new_CA.HalfWorldSize = new_CA.out_Radius + ((FLOAT2FIX2(params[WIDTH_FS]->u.fs_d.value) >> 16) + 2) / 2;
	new_CA.suites = &suites;
	//Create the worlds
	suites.WorldSuite1()->new_world(in_data->effect_ref, new_CA.out_Radius + (FLOAT2FIX2(params[WIDTH_FS]->u.fs_d.value) >> 16) + 2, new_CA.out_Radius + (FLOAT2FIX2(params[WIDTH_FS]->u.fs_d.value) >> 16) + 2, PF_NewWorldFlag_NONE, &number_element1);
	suites.WorldSuite1()->new_world(in_data->effect_ref, new_CA.out_Radius * 2 + (FLOAT2FIX2(params[WIDTH_FS]->u.fs_d.value) >> 16) + 2, new_CA.out_Radius + (FLOAT2FIX2(params[WIDTH_FS]->u.fs_d.value) >> 16) + 2, PF_NewWorldFlag_NONE, &number_element2);
	suites.WorldSuite1()->new_world(in_data->effect_ref, new_CA.out_Radius * 2 + (FLOAT2FIX2(params[WIDTH_FS]->u.fs_d.value) >> 16) + 2, new_CA.out_Radius * 2 + (FLOAT2FIX2(params[WIDTH_FS]->u.fs_d.value) >> 16) + 2, PF_NewWorldFlag_NONE, &number_element4);
	suites.WorldSuite1()->new_world(in_data->effect_ref, new_CA.out_Radius * 2 + (FLOAT2FIX2(params[WIDTH_FS]->u.fs_d.value) >> 16) + 2, new_CA.out_Radius * 2 + (FLOAT2FIX2(params[WIDTH_FS]->u.fs_d.value) >> 16) + 2, PF_NewWorldFlag_CLEAR_PIXELS, &Tmp_dst);
	PF_HLS_Pixel Global_C_HLS;
	PF_Pixel Global_C;
	PF_ColorCallbacksSuite1 *CCsuite = NULL;
	PF_FpLong End_Angle;
	A_u_char used_World = 0;
	PF_FloatMatrix transFormMatrix_Circle;
	PF_Point transform_point1;
	transform_point1.h = new_CA.out_Radius;
	transform_point1.v = new_CA.out_Radius;
	PF_Point transform_point2;
	transform_point2.h = 0;
	transform_point2.v = new_CA.out_Radius;
	PF_FloatMatrix transFormMatrix_line1, transFormMatrix_line2, transFormMatrix_line4;

	if (params[GLOBAL_COLOR_CHKBOX]->u.bd.value) {
		in_data->pica_basicP->AcquireSuite(kPFColorCallbacksSuite, kPFColorCallbacksSuiteVersion1, (const void**)&CCsuite);
		CCsuite->RGBtoHLS(in_data->effect_ref, &params[FILL_COLOR_C]->u.cd.value, Global_C_HLS);
		new_CA.color = &Global_C;
	}
	PF_Rect destination1, destination2, destination4;
	destination1.bottom = (FLOAT2FIX2(params[WIDTH_FS]->u.fs_d.value/2) >> 16) + 1;
	destination1.top = destination1.bottom - 1;
	destination1.left = (FLOAT2FIX2(params[WIDTH_FS]->u.fs_d.value / 2) >> 16) + 1 + (FLOAT2FIX2(params[IN_RADIUS_FS]->u.fs_d.value) >> 16);
	destination1.right = (FLOAT2FIX2(params[WIDTH_FS]->u.fs_d.value / 2) >> 16) + 1 + (FLOAT2FIX2(params[OUT_RADIUS_FS]->u.fs_d.value) >> 16);

	destination2.bottom = (FLOAT2FIX2(params[WIDTH_FS]->u.fs_d.value / 2) >> 16) + 1;
	destination2.top = destination2.bottom - 1;
	destination2.left =  (FLOAT2FIX2(params[IN_RADIUS_FS]->u.fs_d.value) >> 16) + new_CA.HalfWorldSize;
	destination2.right = (FLOAT2FIX2(params[OUT_RADIUS_FS]->u.fs_d.value) >> 16) + new_CA.HalfWorldSize;

	destination4.bottom = new_CA.HalfWorldSize;
	destination4.top = destination4.bottom - 1;
	destination4.left = (FLOAT2FIX2(params[IN_RADIUS_FS]->u.fs_d.value) >> 16) + new_CA.HalfWorldSize;
	destination4.right = (FLOAT2FIX2(params[OUT_RADIUS_FS]->u.fs_d.value) >> 16) + new_CA.HalfWorldSize;


	if (has_line) {
		CCU_SetIdentityMatrix(&transFormMatrix_line1);
		CCU_SetIdentityMatrix(&transFormMatrix_line2);
		CCU_SetIdentityMatrix(&transFormMatrix_line4);
		CCU_TransformPointsFloat(&transFormMatrix_line1,params[IN_RADIUS_FS]->u.fs_d.value, 0);
		CCU_TransformPointsFloat(&transFormMatrix_line2, params[OUT_RADIUS_FS]->u.fs_d.value + params[IN_RADIUS_FS]->u.fs_d.value,0);
		CCU_TransformPointsFloat(&transFormMatrix_line4, params[OUT_RADIUS_FS]->u.fs_d.value + params[IN_RADIUS_FS]->u.fs_d.value, params[OUT_RADIUS_FS]->u.fs_d.value);
	}


	/*Enter LOOP for all the numbers*/
	PF_FpLong start_Angle = 0;
	if (params[GLOBAL_COLOR_CHKBOX]->u.bd.value) {
		PF_FpLong phase =  (params[PHASE_OFFSET_FS]->u.fs_d.value) * 360 / total_num /100;
		if (params[PHASE_POPUP]->u.pd.value != 1) {
			phase = (params[PHASE_OFFSET_FS]->u.fs_d.value)  / total_num / 100;
		}
		PF_FpLong ChangeCValue = FIX_2_FLOAT(Global_C_HLS[params[PHASE_POPUP]->u.pd.value - 1]);
		for (now = head, head = NULL; now != NULL; now = now->next) {
			delete head;
			//Iterate(Fill) 扇形 PF_EffectWorld, optimize size according to angle(0~90, 90~180, 180~360) 
			//Calculate Angle Needed
			End_Angle = ((now->number / sumV)*params[FULL_PERCENT_FS]->u.fs_d.value / 100)*PF_2PI;
			new_CA.angle = End_Angle;
			//Get now color

			if (params[FILL_STROKE_POPUP]->u.pd.value != 2) { //Need FILL??
				CCsuite->HLStoRGB(in_data->effect_ref, Global_C_HLS, new_CA.color);
				//Change HLS Value
				if (params[PLUS_MINUS_POPUP]->u.pd.value == 1) {
					ChangeCValue += phase;
					if (params[PHASE_POPUP]->u.pd.value != 1) {
						if (ChangeCValue > 1) ChangeCValue -= 1;
					}
					else {
						if (ChangeCValue > 360) ChangeCValue -= 360;
					}
				}
				else {
					ChangeCValue -= phase;
					if (params[PHASE_POPUP]->u.pd.value != 1) {
						if (ChangeCValue < 0) ChangeCValue += 1;
					}
					else {
						if (ChangeCValue < 0) ChangeCValue += 360;
					}
				}
				new_CA.color->alpha = 255;
				Global_C_HLS[params[PHASE_POPUP]->u.pd.value - 1] = FLOAT2FIX2(ChangeCValue);
				//if(Global_C_HLS[params[PHASE_POPUP]->u.pd.value-1] > (255<<16)) Global_C_HLS[params[PHASE_POPUP]->u.pd.value-1] -= (255 << 16);

				//FILLING
				if (End_Angle==0) {
					suites.FillMatteSuite2()->fill(in_data->effect_ref, NULL, NULL, &number_element1);
					used_World = 1;
				}
				else if (End_Angle < PF_PI / 2) {
					suites.Iterate8Suite1()->iterate(in_data, 0, 0, NULL, NULL, &new_CA, &Make_Num_Part_Pix1, &number_element1);
					suites.FillMatteSuite2()->fill(in_data->effect_ref, new_CA.color, &destination1, &number_element1);
					used_World = 1;
				}
				else if (End_Angle < PF_PI) {
					suites.Iterate8Suite1()->iterate(in_data, 0, 0, NULL, NULL, &new_CA, &Make_Num_Part_Pix2, &number_element2);
					suites.FillMatteSuite2()->fill(in_data->effect_ref, new_CA.color, &destination2, &number_element2);
					used_World = 2;
				}
				else {
					suites.Iterate8Suite1()->iterate(in_data, 0, 0, NULL, NULL, &new_CA, &Make_Num_Part_Pix4, &number_element4);
					suites.FillMatteSuite2()->fill(in_data->effect_ref, new_CA.color, &destination4, &number_element4);
					used_World = 4;
				}
				
				
			}
			else {
				suites.FillMatteSuite2()->fill(in_data->effect_ref, NULL, NULL, &number_element1);
				used_World = 1;
			}
			//Concat Stroke onto the 扇形 if necessary
			if (has_line) {
				switch (used_World) {
				case 1:
					suites.WorldTransformSuite1()->transform_world(in_data->effect_ref, in_data->quality, PF_MF_Alpha_PREMUL, PF_Field_FRAME, &line,
						&Normal_CmpMode, NULL, &transFormMatrix_line1, 1, true, &number_element1.extent_hint, &number_element1);
					break;
				case 2:
					suites.WorldTransformSuite1()->transform_world(in_data->effect_ref, in_data->quality, PF_MF_Alpha_PREMUL, PF_Field_FRAME, &line,
						&Normal_CmpMode, NULL, &transFormMatrix_line2, 1, true, &number_element2.extent_hint, &number_element2);
					break;
				case 4:
					suites.WorldTransformSuite1()->transform_world(in_data->effect_ref, in_data->quality, PF_MF_Alpha_PREMUL, PF_Field_FRAME, &line,
						&Normal_CmpMode, NULL, &transFormMatrix_line4, 1, true, &number_element4.extent_hint, &number_element4);
					break;
				default:
					break;
				}
			}
			

			//rotate for graph and print to Tmp_dst
			CCU_SetIdentityMatrix(&transFormMatrix_Circle);
			switch (used_World) {
			case 1:
				CCU_RotateMatrixPlus(&transFormMatrix_Circle, in_data, start_Angle, (FLOAT2FIX2(params[WIDTH_FS]->u.fs_d.value) >> 16) / 2 + 1, (FLOAT2FIX2(params[WIDTH_FS]->u.fs_d.value) >> 16) / 2 + 1);
				CCU_TransformPoints(&transFormMatrix_Circle, &transform_point1);
				suites.WorldTransformSuite1()->transform_world(in_data->effect_ref, in_data->quality, PF_MF_Alpha_PREMUL, PF_Field_FRAME, &number_element1,
					&Normal_CmpMode, NULL, &transFormMatrix_Circle, 1, true, &Tmp_dst.extent_hint, &Tmp_dst);
				break;
			case 2:
				CCU_RotateMatrixPlus(&transFormMatrix_Circle, in_data, start_Angle, new_CA.HalfWorldSize, (FLOAT2FIX2(params[WIDTH_FS]->u.fs_d.value) >> 16) / 2 + 1);
				CCU_TransformPoints(&transFormMatrix_Circle, &transform_point2);
				suites.WorldTransformSuite1()->transform_world(in_data->effect_ref, in_data->quality, PF_MF_Alpha_PREMUL, PF_Field_FRAME, &number_element2,
					&Normal_CmpMode, NULL, &transFormMatrix_Circle, 1, true, &Tmp_dst.extent_hint, &Tmp_dst);
				break;
			case 4:
				CCU_RotateMatrixPlus(&transFormMatrix_Circle, in_data, start_Angle, new_CA.HalfWorldSize, new_CA.HalfWorldSize);
				suites.WorldTransformSuite1()->transform_world(in_data->effect_ref, in_data->quality, PF_MF_Alpha_PREMUL, PF_Field_FRAME, &number_element4,
					&Normal_CmpMode, NULL, &transFormMatrix_Circle, 1, true, &Tmp_dst.extent_hint, &Tmp_dst);
				break;
			default:
				suites.FillMatteSuite2()->fill(in_data->effect_ref, new_CA.color, NULL, &Tmp_dst);
				break;
			}
			start_Angle += End_Angle;
			head = now;
		}
		//concat last line to Tmp_dst
		if (has_line) {
			CCU_RotateMatrixPlus(&transFormMatrix_line1, in_data, start_Angle, (FLOAT2FIX2(params[WIDTH_FS]->u.fs_d.value) >> 16) / 2 + 1, (FLOAT2FIX2(params[WIDTH_FS]->u.fs_d.value) >> 16) / 2 + 1);
			CCU_TransformPoints(&transFormMatrix_line1, &transform_point1);
			suites.WorldTransformSuite1()->transform_world(in_data->effect_ref, in_data->quality, PF_MF_Alpha_PREMUL, PF_Field_FRAME, &line,
				&Normal_CmpMode, NULL, &transFormMatrix_line1, 1, true, &Tmp_dst.extent_hint, &Tmp_dst);
		}
	}
	else {
		for (now_C = head_C, head_C = NULL; now_C != NULL; now_C = now_C->next) {
			delete head_C;
			//Iterate(Fill) 扇形 PF_EffectWorld, optimize size according to angle(0~90, 90~180, 180~360) 
			//Calculate Angle Needed
			End_Angle = ((now_C->number / sumV)*params[FULL_PERCENT_FS]->u.fs_d.value / 100)*PF_2PI;
			new_CA.angle = End_Angle;
			
			if (params[FILL_STROKE_POPUP]->u.pd.value != 2) { //Need FILL??
			//Get now_C color
			new_CA.color = &now_C->Color;
			//FILLING
				if (End_Angle == 0) {
					suites.FillMatteSuite2()->fill(in_data->effect_ref, NULL, NULL, &number_element1);
					used_World = 1;
				}
				else if (End_Angle < PF_PI / 2) {
					suites.Iterate8Suite1()->iterate(in_data, 0, 0, NULL, NULL, &new_CA, &Make_Num_Part_Pix1, &number_element1);
					suites.FillMatteSuite2()->fill(in_data->effect_ref, new_CA.color, &destination1 , &number_element1);
					used_World = 1;
				}
				else if (End_Angle < PF_PI) {
					suites.Iterate8Suite1()->iterate(in_data, 0, 0, NULL, NULL, &new_CA, &Make_Num_Part_Pix2, &number_element2);
					suites.FillMatteSuite2()->fill(in_data->effect_ref, new_CA.color, &destination2, &number_element2);
					used_World = 2;
				}
				else {
					suites.Iterate8Suite1()->iterate(in_data, 0, 0, NULL, NULL, &new_CA, &Make_Num_Part_Pix4, &number_element4);
					suites.FillMatteSuite2()->fill(in_data->effect_ref, new_CA.color, &destination4, &number_element4);
					used_World = 4;
				}
			}
			else {
				suites.FillMatteSuite2()->fill(in_data->effect_ref, NULL, NULL, &number_element1);
				used_World = 1;
			}

			//Concat Stroke onto the 扇形 if necessary
			if (has_line) {
				switch (used_World) {
				case 1:
					suites.WorldTransformSuite1()->transform_world(in_data->effect_ref, in_data->quality, PF_MF_Alpha_PREMUL, PF_Field_FRAME, &line,
						&Normal_CmpMode, NULL, &transFormMatrix_line1, 1, true, &number_element1.extent_hint, &number_element1);
					break;
				case 2:
					suites.WorldTransformSuite1()->transform_world(in_data->effect_ref, in_data->quality, PF_MF_Alpha_PREMUL, PF_Field_FRAME, &line,
						&Normal_CmpMode, NULL, &transFormMatrix_line2, 1, true, &number_element2.extent_hint, &number_element2);
					break;
				case 4:
					suites.WorldTransformSuite1()->transform_world(in_data->effect_ref, in_data->quality, PF_MF_Alpha_PREMUL, PF_Field_FRAME, &line,
						&Normal_CmpMode, NULL, &transFormMatrix_line4, 1, true, &number_element4.extent_hint, &number_element4);
					break;
				default:
					break;
				}
			}
			//rotate for graph and print to Tmp_dst
			CCU_SetIdentityMatrix(&transFormMatrix_Circle);
			switch (used_World) {
			case 1:
				CCU_RotateMatrixPlus(&transFormMatrix_Circle, in_data, start_Angle, (FLOAT2FIX2(params[WIDTH_FS]->u.fs_d.value) >> 16) / 2 + 1, (FLOAT2FIX2(params[WIDTH_FS]->u.fs_d.value) >> 16) / 2 + 1);
				CCU_TransformPoints(&transFormMatrix_Circle, &transform_point1);
				suites.WorldTransformSuite1()->transform_world(in_data->effect_ref, in_data->quality, PF_MF_Alpha_PREMUL, PF_Field_FRAME, &number_element1,
					&Normal_CmpMode, NULL, &transFormMatrix_Circle, 1, true, &Tmp_dst.extent_hint, &Tmp_dst);
				break;
			case 2:
				CCU_RotateMatrixPlus(&transFormMatrix_Circle, in_data, start_Angle, new_CA.HalfWorldSize, (FLOAT2FIX2(params[WIDTH_FS]->u.fs_d.value) >> 16) / 2 + 1);
				CCU_TransformPoints(&transFormMatrix_Circle, &transform_point2);
				suites.WorldTransformSuite1()->transform_world(in_data->effect_ref, in_data->quality, PF_MF_Alpha_PREMUL, PF_Field_FRAME, &number_element2,
					&Normal_CmpMode, NULL, &transFormMatrix_Circle, 1, true, &Tmp_dst.extent_hint, &Tmp_dst);
				break;
			case 4:
				CCU_RotateMatrixPlus(&transFormMatrix_Circle, in_data, start_Angle, new_CA.HalfWorldSize, new_CA.HalfWorldSize);
				suites.WorldTransformSuite1()->transform_world(in_data->effect_ref, in_data->quality, PF_MF_Alpha_PREMUL, PF_Field_FRAME, &number_element4,
					&Normal_CmpMode, NULL, &transFormMatrix_Circle, 1, true, &Tmp_dst.extent_hint, &Tmp_dst);
				break;
			default:
				suites.FillMatteSuite2()->fill(in_data->effect_ref, new_CA.color, NULL, &Tmp_dst);
				break;
			}
			start_Angle += End_Angle;
			head_C = now_C;
		}
		//concat last line to Tmp_dst
		if (has_line) {
			CCU_RotateMatrixPlus(&transFormMatrix_line1, in_data, start_Angle, (FLOAT2FIX2(params[WIDTH_FS]->u.fs_d.value) >> 16) / 2 + 1, (FLOAT2FIX2(params[WIDTH_FS]->u.fs_d.value) >> 16) / 2 + 1);
			CCU_TransformPoints(&transFormMatrix_line1, &transform_point1);
			suites.WorldTransformSuite1()->transform_world(in_data->effect_ref, in_data->quality, PF_MF_Alpha_PREMUL, PF_Field_FRAME, &line,
				&Normal_CmpMode, NULL, &transFormMatrix_line1, 1, true, &Tmp_dst.extent_hint, &Tmp_dst);
		}
	}
	/*exit LOOP*/
	if ((params[FILL_STROKE_POPUP]->u.pd.value == 2 || params[FILL_STROKE_POPUP]->u.pd.value == 3) && (params[WIDTH_FS]->u.fs_d.value != 0)) {
		//concat stroke for inner and outer circle
		if (params[IN_RADIUS_FS]->u.fs_d.value != 0 && params[IN_RADIUS_FS]->u.fs_d.value<params[OUT_RADIUS_FS]->u.fs_d.value) {
			//Calculate radius for each stroke element
			PF_FpLong move_radius_out = params[SPACING_FS]->u.fs_d.value / params[OUT_RADIUS_FS]->u.fs_d.value;
			PF_FpLong move_radius_in = params[SPACING_FS]->u.fs_d.value / params[IN_RADIUS_FS]->u.fs_d.value;
			CCU_SetIdentityMatrix(&transFormMatrix_line1);
			CCU_TransformPointsFloat(&transFormMatrix_line1, params[OUT_RADIUS_FS]->u.fs_d.value, 0);
			CCU_RotateMatrixPlus(&transFormMatrix_line1, in_data, PF_HALF_PI, new_CA.HalfWorldSize, new_CA.HalfWorldSize);
			for (PF_FpLong now = 0; now < start_Angle; now += move_radius_out) {
				suites.WorldTransformSuite1()->transform_world(in_data->effect_ref, in_data->quality, PF_MF_Alpha_PREMUL, PF_Field_FRAME, &stroke_element,
					&Normal_CmpMode, NULL, &transFormMatrix_line1, 1, true, &Tmp_dst.extent_hint, &Tmp_dst);
				CCU_RotateMatrixPlus(&transFormMatrix_line1, in_data, move_radius_out, new_CA.HalfWorldSize, new_CA.HalfWorldSize);
			}
			CCU_SetIdentityMatrix(&transFormMatrix_line1);
			CCU_TransformPointsFloat(&transFormMatrix_line1, params[OUT_RADIUS_FS]->u.fs_d.value, params[OUT_RADIUS_FS]->u.fs_d.value - params[IN_RADIUS_FS]->u.fs_d.value);
			CCU_RotateMatrixPlus(&transFormMatrix_line1, in_data, PF_HALF_PI, new_CA.HalfWorldSize, new_CA.HalfWorldSize);
			for (PF_FpLong now = 0; now < start_Angle; now += move_radius_in) {	
				suites.WorldTransformSuite1()->transform_world(in_data->effect_ref, in_data->quality, PF_MF_Alpha_PREMUL, PF_Field_FRAME, &stroke_element,
					&Normal_CmpMode, NULL, &transFormMatrix_line1, 1, true, &Tmp_dst.extent_hint, &Tmp_dst);
				CCU_RotateMatrixPlus(&transFormMatrix_line1, in_data, move_radius_in, new_CA.HalfWorldSize, new_CA.HalfWorldSize);
			}
		}
		else {//outer only!
			PF_FpLong move_radius_out = params[SPACING_FS]->u.fs_d.value / params[OUT_RADIUS_FS]->u.fs_d.value;
			CCU_SetIdentityMatrix(&transFormMatrix_line1);
			CCU_TransformPointsFloat(&transFormMatrix_line1, params[OUT_RADIUS_FS]->u.fs_d.value, 0);
			CCU_RotateMatrixPlus(&transFormMatrix_line1, in_data, PF_HALF_PI, new_CA.HalfWorldSize, new_CA.HalfWorldSize);
			for (PF_FpLong now = 0; now < start_Angle; now += move_radius_out) {
				suites.WorldTransformSuite1()->transform_world(in_data->effect_ref, in_data->quality, PF_MF_Alpha_PREMUL, PF_Field_FRAME, &stroke_element,
					&Normal_CmpMode, NULL, &transFormMatrix_line1, 1, true, &Tmp_dst.extent_hint, &Tmp_dst);
				CCU_RotateMatrixPlus(&transFormMatrix_line1, in_data, move_radius_out, new_CA.HalfWorldSize, new_CA.HalfWorldSize);
			}
		}
		suites.WorldSuite1()->dispose_world(in_data->effect_ref, &stroke_element);
	}
	if (has_line) {
		suites.WorldSuite1()->dispose_world(in_data->effect_ref, &line);
	}
	suites.WorldSuite1()->dispose_world(in_data->effect_ref, &number_element1);
	suites.WorldSuite1()->dispose_world(in_data->effect_ref, &number_element2);
	suites.WorldSuite1()->dispose_world(in_data->effect_ref, &number_element4);

	/* globalRotate, (Scale for downSampling and aspect ratio) and printout to output*/
	//suites.FillMatteSuite2()->fill(in_data->effect_ref, &params[FILL_COLOR_C]->u.cd.value, NULL, &Tmp_dst);

	CCU_SetIdentityMatrix(&transFormMatrix_Circle);
	CCU_RotateMatrixPlus(&transFormMatrix_Circle, in_data,FIX_2_FLOAT( params[ROTATION_ANG]->u.ad.value - (90<<16)) * PF_RAD_PER_DEGREE, new_CA.HalfWorldSize, new_CA.HalfWorldSize);
	CCU_ScaleMatrix(&transFormMatrix_Circle, (PF_FpLong)in_data->downsample_x.num / (PF_FpLong)in_data->downsample_x.den/ (PF_FpLong)in_data->pixel_aspect_ratio.num * (PF_FpLong)in_data->pixel_aspect_ratio.den,
		(PF_FpLong)in_data->downsample_y.num / (PF_FpLong)in_data->downsample_y.den ,
		new_CA.HalfWorldSize, new_CA.HalfWorldSize);


	CCU_TransformPointsFloat(&transFormMatrix_Circle, FIX_2_FLOAT(params[START_POINT]->u.td.x_value - ((Tmp_dst.width<<16)/2)), FIX_2_FLOAT(params[START_POINT]->u.td.y_value - ((Tmp_dst.height<<16)/2)));
	suites.WorldTransformSuite1()->transform_world(in_data->effect_ref, in_data->quality, PF_MF_Alpha_PREMUL, PF_Field_FRAME, &Tmp_dst,
		&Normal_CmpMode, NULL, &transFormMatrix_Circle, 1, true, &output->extent_hint, output);
	/*Done*/
	suites.WorldSuite1()->dispose_world(in_data->effect_ref, &Tmp_dst);
	
	return err;
}

DllExport
PF_Err
EntryPointFuncR(
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

			err = About_R(in_data,
				out_data,
				params,
				output);
			break;

		case PF_Cmd_GLOBAL_SETUP:

			err = GlobalSetup_R(in_data,
				out_data,
				params,
				output);
			break;

		case PF_Cmd_PARAMS_SETUP:

			err = ParamsSetup_R(in_data,
				out_data,
				params,
				output);
			break;

		case PF_Cmd_RENDER:
			err = RenderRING(in_data,
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


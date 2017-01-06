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
About_L(
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
static AEGP_PluginID myAEGPID_L;
static AEGP_InstalledEffectKey key_NUM_L = 0, key_MAIN_L = 0, key_LINK_L = 0;
static PF_Err
GlobalSetup_L(
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
	out_data->out_flags = PF_OutFlag_I_AM_OBSOLETE | PF_OutFlag_NON_PARAM_VARY| PF_OutFlag_PIX_INDEPENDENT;
	out_data->out_flags2 = PF_OutFlag2_PARAM_GROUP_START_COLLAPSED_FLAG| PF_OutFlag2_I_USE_TIMECODE;
	AEGP_SuiteHandler suites(in_data->pica_basicP);
	suites.UtilitySuite5()->AEGP_RegisterWithAEGP(NULL, out_data->name, &myAEGPID_L);
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
			key_NUM_L = keyTMP;
		}
		else if (!strcmp(name_MAIN, matchNameTMP)) {
			key_MAIN_L = keyTMP;
		}
		else if (!strcmp(name_LINK, matchNameTMP)) {
			key_LINK_L = keyTMP;
		}
		if ((key_NUM_L != 0) && (key_MAIN_L != 0) && (key_LINK_L != 0)) {
			break;
		}
		now++;
	}

	return PF_Err_NONE;
}

enum PARAMS_L {
	START_POINT = 1,
	MAXHEIGHT_FS,
	STANDARD_FS,
	SPACING_FS,

	DOT_GROUPSTART,
	RENDOT_CB,
	DOT_WIDTH_FS,
	USE_GCOLOR_CBOX,
	COLOR_C,
	DOT_FEATHER_FS,
	DOT_GROUPEND,

	LINE_GROUPSTART,
	RENLINE_CB,
	LINE_COLOR_C,
	LINE_WIDTH_FS,
	LINE_FEATHER_FS,
	LINE_GROUPEND,
	
	FACE_GROUPSTART,
	RENFACE_CB,
	FACE_COLOR_C,
	FACE_OPACITY_FS,
	FACE_GROUPEND,

	COMPOSIT_POPUP
};

static PF_Err
ParamsSetup_L(
	PF_InData		*in_data,
	PF_OutData		*out_data,
	PF_ParamDef		*params[],
	PF_LayerDef		*output)
{
	PF_Err		err = PF_Err_NONE;
	PF_ParamDef	def;

	AEFX_CLR_STRUCT(def);
	out_data->num_params = 1;

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

	def.param_type = PF_Param_FLOAT_SLIDER;
	PF_STRCPY(def.name, "Max. Height");
	def.uu.id = 3;
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
	def.uu.id = 4;
	def.u.fs_d.precision = 1;
	def.u.fs_d.dephault = 100;
	def.u.fs_d.slider_min = (PF_FpShort)0.01;
	def.u.fs_d.slider_max = 2000;
	def.u.fs_d.valid_max = 36000000;
	def.u.fs_d.valid_min = (PF_FpShort) 0.01;
	def.u.fs_d.display_flags = 0;
	if (err = PF_ADD_PARAM(in_data, -1, &def))
		return err;
	AEFX_CLR_STRUCT(def);
	out_data->num_params++;

	def.param_type = PF_Param_FLOAT_SLIDER;
	PF_STRCPY(def.name, "Spacing");
	def.uu.id = 5;
	def.u.fs_d.precision = 0;
	def.u.fs_d.dephault = 80;
	def.u.fs_d.slider_min = 0;
	def.u.fs_d.slider_max = 200;
	def.u.fs_d.valid_max = 36000000;
	def.u.fs_d.valid_min = 1;
	def.u.fs_d.display_flags = 0;
	if (err = PF_ADD_PARAM(in_data, -1, &def))
		return err;
	AEFX_CLR_STRUCT(def);
	out_data->num_params++;

	def.param_type = PF_Param_GROUP_START;
	PF_STRCPY(def.name, "Point(s)");
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
	PF_STRCPY(name, "Show Point(s)");
	def.u.bd.u.nameptr = name;
	if (err = PF_ADD_PARAM(in_data, -1, &def))
		return err;
	AEFX_CLR_STRUCT(def);
	out_data->num_params++;


	def.param_type = PF_Param_FLOAT_SLIDER;
	PF_STRCPY(def.name, "Point Width");
	def.uu.id = 8;
	def.u.fs_d.precision = 0;
	def.u.fs_d.dephault = 20;
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
	def.uu.id = 9;
	def.u.bd.dephault = true;
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
	def.uu.id = 10;
	def.flags = PF_ParamFlag_NONE;
	if (err = PF_ADD_PARAM(in_data, -1, &def))
		return err;
	AEFX_CLR_STRUCT(def);
	out_data->num_params++;

	def.param_type = PF_Param_FLOAT_SLIDER;
	PF_STRCPY(def.name, "Point Feather");
	def.uu.id = 24;
	def.u.fs_d.dephault = 10;
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
	PF_STRCPY(def.name, "Line(s)");
	def.uu.id = 12;
	def.flags = PF_ParamFlag_START_COLLAPSED;
	if (err = PF_ADD_PARAM(in_data, -1, &def))
		return err;
	AEFX_CLR_STRUCT(def);
	out_data->num_params++;

	def.param_type = PF_Param_CHECKBOX;
	PF_STRCPY(def.name, "");
	def.uu.id = 13;
	def.u.bd.dephault = true;
	PF_STRCPY(name, "Show Line(s)");
	def.u.bd.u.nameptr = name;
	if (err = PF_ADD_PARAM(in_data, -1, &def))
		return err;
	AEFX_CLR_STRUCT(def);
	out_data->num_params++;

	def.param_type = PF_Param_COLOR;
	PF_STRCPY(def.name, "Line Color");
	def.u.cd.dephault.alpha = 255;
	def.u.cd.dephault.red = 0;
	def.u.cd.dephault.blue = 0;
	def.u.cd.dephault.green = 255;
	def.u.cd.value = def.u.cd.dephault;
	def.uu.id = 14;
	def.flags = PF_ParamFlag_NONE;
	if (err = PF_ADD_PARAM(in_data, -1, &def))
		return err;
	AEFX_CLR_STRUCT(def);
	out_data->num_params++;

	def.param_type = PF_Param_FLOAT_SLIDER;
	PF_STRCPY(def.name, "Line Width");
	def.uu.id = 15;
	def.u.fs_d.precision = 0;
	def.u.fs_d.dephault = 5;
	def.u.fs_d.slider_min = 0;
	def.u.fs_d.slider_max = 200;
	def.u.fs_d.valid_max = 36000000;
	def.u.fs_d.valid_min = 0;
	def.u.fs_d.display_flags = 0;
	if (err = PF_ADD_PARAM(in_data, -1, &def))
		return err;
	AEFX_CLR_STRUCT(def);
	out_data->num_params++;

	def.param_type = PF_Param_FLOAT_SLIDER;
	PF_STRCPY(def.name, "Line Feather");
	def.uu.id = 16;
	def.u.fs_d.dephault = 10;
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
	def.uu.id = 17;
	if (err = PF_ADD_PARAM(in_data, -1, &def))
		return err;
	AEFX_CLR_STRUCT(def);
	out_data->num_params++;


	def.param_type = PF_Param_GROUP_START;
	PF_STRCPY(def.name, "Face");
	def.uu.id = 18;
	def.flags = PF_ParamFlag_START_COLLAPSED;
	if (err = PF_ADD_PARAM(in_data, -1, &def))
		return err;
	AEFX_CLR_STRUCT(def);
	out_data->num_params++;

	def.param_type = PF_Param_CHECKBOX;
	PF_STRCPY(def.name, "");
	def.uu.id = 19;
	def.u.bd.dephault = true;
	PF_STRCPY(name, "Show Face");
	def.u.bd.u.nameptr = name;
	if (err = PF_ADD_PARAM(in_data, -1, &def))
		return err;
	AEFX_CLR_STRUCT(def);
	out_data->num_params++;

	def.param_type = PF_Param_COLOR;
	PF_STRCPY(def.name, "Face Color");
	def.u.cd.dephault.alpha = 255;
	def.u.cd.dephault.red = 0;
	def.u.cd.dephault.blue = 0;
	def.u.cd.dephault.green = 255;
	def.u.cd.value = def.u.cd.dephault;
	def.uu.id = 20;
	def.flags = PF_ParamFlag_NONE;
	if (err = PF_ADD_PARAM(in_data, -1, &def))
		return err;
	AEFX_CLR_STRUCT(def);
	out_data->num_params++;

	def.param_type = PF_Param_FLOAT_SLIDER;
	PF_STRCPY(def.name, "Face Opacity");
	def.uu.id = 21;
	def.u.fs_d.dephault = 50;
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
	def.uu.id = 22;
	if (err = PF_ADD_PARAM(in_data, -1, &def))
		return err;
	AEFX_CLR_STRUCT(def);
	out_data->num_params++;

	A_char PopupStr_2[50];
	PF_STRCPY(PopupStr_2, "On Original Layer|On Transparent");
	def.param_type = PF_Param_POPUP;
	PF_STRCPY(def.name, "Composite Mode");
	def.uu.id = 23;
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

//Functions for render
typedef struct color_feather {
	PF_Pixel color;
	A_long Radius;
	A_long Start_Feather_Radius;
} Color_Feather;


PF_Err Make_Dot_Pix(void *refcon, A_long x, A_long y, PF_Pixel *in, PF_Pixel *out) {
	Color_Feather *C_F = (Color_Feather*)refcon;
	x -= C_F->Radius + 1;
	y -= C_F->Radius + 1;
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
		}
		else if ((C_F->Start_Feather_Radius)*(C_F->Start_Feather_Radius) > (x*x + y*y)) {
			out->alpha = 255;
		}
		else {
			out->alpha = (A_u_char)(255 - (255 * (x*x + y*y - C_F->Start_Feather_Radius*C_F->Start_Feather_Radius) / ((C_F->Radius)*(C_F->Radius) - C_F->Start_Feather_Radius*C_F->Start_Feather_Radius)));
		}
	}
	return A_Err_NONE;
}

PF_Err Make_Segment_Pix(void *refcon, A_long x, A_long y, PF_Pixel *in, PF_Pixel *out) {
	Color_Feather *C_F = (Color_Feather*)refcon;
	out->red = C_F->color.red;
	out->green = C_F->color.green;
	out->blue = C_F->color.blue;
	y -= C_F->Radius + 1;
	if (C_F->Start_Feather_Radius == C_F->Radius) {
		out->alpha = 255;
		return A_Err_NONE;
	}
	else if (ABS(y) < C_F->Start_Feather_Radius) {
		out->alpha = 255;
	}
	else if (ABS(y) > C_F->Radius) {
		out->alpha = 0;
	}
	else {
		out->alpha = (A_u_char)(255- (255 * (ABS(y) - C_F->Start_Feather_Radius) / (C_F->Radius - C_F->Start_Feather_Radius)));
	}
	return A_Err_NONE;
}

typedef struct face_struct {
	PF_Pixel color;
	PF_FpLong Start_y;
	PF_FpLong Start_x;
	PF_FpLong slope;
} Face_Struct;

PF_Err Make_Face_Pix(void *refcon, A_long x, A_long y, PF_Pixel *in, PF_Pixel *out) {
	Face_Struct* F_S = (Face_Struct*)refcon;
	PF_FpLong tmp = (F_S->Start_y + ((PF_FpLong)x - F_S->Start_x)*F_S->slope);
	out->red = F_S->color.red;
	out->green = F_S->color.green;
	out->blue = F_S->color.blue;
	if (tmp < y) {
		out->alpha = F_S->color.alpha;
	}else if (tmp - (PF_FpLong)y < ABS(F_S->slope)) {
		out->alpha = (char)(F_S->color.alpha - ((PF_FpLong)F_S->color.alpha * (tmp - (PF_FpLong)y)/ ABS(F_S->slope) ));
	}
	else {
		out->alpha = 0;
	}
	return A_Err_NONE;
}

static PF_Err
RenderLINE(
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
	suites.PFInterfaceSuite1()->AEGP_GetNewEffectForEffect(myAEGPID_L, in_data->effect_ref, &myEffectRefH);
	suites.StreamSuite4()->AEGP_GetNewEffectStreamByIndex(myAEGPID_L, myEffectRefH, 1, &myStreamH);
	suites.DynamicStreamSuite4()->AEGP_GetNewParentStreamRef(myAEGPID_L, myStreamH, &myEffStreamH);
	suites.DynamicStreamSuite4()->AEGP_GetStreamIndexInParent(myEffStreamH, &Eff_index);
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
	A_long NumCount = 0;
	suites.PFInterfaceSuite1()->AEGP_ConvertEffectToCompTime(in_data->effect_ref, in_data->current_time, in_data->time_scale, &current_time);
	if (params[USE_GCOLOR_CBOX]->u.bd.value) {
		A_long index = 0;
		AEGP_InstalledEffectKey ChkKey;
		AEGP_StreamRefH tmpStreamH;
		AEGP_StreamValue2 tmpValue;
		for (index = 0; index < Eff_index; index++) {
			suites.EffectSuite3()->AEGP_GetLayerEffectByIndex(myAEGPID_L, LayerH, index, &tmpEffectRefH);
			suites.EffectSuite3()->AEGP_GetInstalledKeyFromLayerEffect(tmpEffectRefH, &ChkKey);
			if (ChkKey == key_NUM_L) {
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
				suites.StreamSuite4()->AEGP_GetNewEffectStreamByIndex(myAEGPID_L, tmpEffectRefH, 1, &tmpStreamH);
				suites.StreamSuite4()->AEGP_GetNewStreamValue(myAEGPID_L, tmpStreamH, AEGP_LTimeMode_CompTime, &current_time, false, &tmpValue);
				now->number = tmpValue.val.one_d;
				if (now->number > maxV) maxV = now->number;
				suites.StreamSuite4()->AEGP_DisposeStreamValue(&tmpValue);
				suites.StreamSuite4()->AEGP_DisposeStream(tmpStreamH);
				NumCount++;
			}
			else if (ChkKey == key_LINK_L) {
				suites.StreamSuite4()->AEGP_GetNewEffectStreamByIndex(myAEGPID_L, tmpEffectRefH, 1, &tmpStreamH);
				suites.StreamSuite4()->AEGP_GetNewStreamValue(myAEGPID_L, tmpStreamH, AEGP_LTimeMode_CompTime, &current_time, false, &tmpValue);
				AEGP_CompH CompH = NULL;
				AEGP_LayerH Link_LayerH = NULL;
				AEGP_EffectRefH Link_tmpEffectRefH = NULL;
				A_long link_eff_index = 0;
				suites.LayerSuite7()->AEGP_GetLayerParentComp(LayerH, &CompH);
				if (tmpValue.val.layer_id != 0) {
					suites.LayerSuite7()->AEGP_GetLayerFromLayerID(CompH, tmpValue.val.layer_id, &Link_LayerH);
					suites.EffectSuite3()->AEGP_GetLayerNumEffects(Link_LayerH, &link_eff_index);
					for (A_long Link_index = 0; Link_index < link_eff_index; Link_index++) {
						suites.EffectSuite3()->AEGP_GetLayerEffectByIndex(myAEGPID_L, Link_LayerH, Link_index, &Link_tmpEffectRefH);
						suites.EffectSuite3()->AEGP_GetInstalledKeyFromLayerEffect(Link_tmpEffectRefH, &ChkKey);
						if (ChkKey == key_NUM_L) {
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
							suites.StreamSuite4()->AEGP_GetNewEffectStreamByIndex(myAEGPID_L, Link_tmpEffectRefH, 1, &tmpStreamH);
							suites.StreamSuite4()->AEGP_GetNewStreamValue(myAEGPID_L, tmpStreamH, AEGP_LTimeMode_CompTime, &current_time, false, &tmpValue);
							now->number = tmpValue.val.one_d;
							if (now->number > maxV) maxV = now->number;
							suites.StreamSuite4()->AEGP_DisposeStreamValue(&tmpValue);
							suites.StreamSuite4()->AEGP_DisposeStream(tmpStreamH);
							NumCount++;
						}
						suites.EffectSuite3()->AEGP_DisposeEffect(Link_tmpEffectRefH);
					}
				}

			}
			suites.EffectSuite3()->AEGP_DisposeEffect(tmpEffectRefH);
		}

	}
	else {
		A_long index = 0;
		AEGP_InstalledEffectKey ChkKey;
		AEGP_StreamRefH tmpStreamH;
		AEGP_StreamValue2 tmpValue;

		for (index = 0; index < Eff_index; index++) {
			suites.EffectSuite3()->AEGP_GetLayerEffectByIndex(myAEGPID_L, LayerH, index, &tmpEffectRefH);
			suites.EffectSuite3()->AEGP_GetInstalledKeyFromLayerEffect(tmpEffectRefH, &ChkKey);
			if (ChkKey == key_NUM_L) {
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
				suites.StreamSuite4()->AEGP_GetNewEffectStreamByIndex(myAEGPID_L, tmpEffectRefH, 1, &tmpStreamH);
				suites.StreamSuite4()->AEGP_GetNewStreamValue(myAEGPID_L, tmpStreamH, AEGP_LTimeMode_CompTime, &current_time, false, &tmpValue);
				now_C->number = tmpValue.val.one_d;
				if (now_C->number > maxV) maxV = now_C->number;
				suites.StreamSuite4()->AEGP_DisposeStreamValue(&tmpValue);
				suites.StreamSuite4()->AEGP_DisposeStream(tmpStreamH);

				suites.StreamSuite4()->AEGP_GetNewEffectStreamByIndex(myAEGPID_L, tmpEffectRefH, 2, &tmpStreamH);
				suites.StreamSuite4()->AEGP_GetNewStreamValue(myAEGPID_L, tmpStreamH, AEGP_LTimeMode_CompTime, &current_time, false, &tmpValue);
				now_C->Color.alpha = 255;
				now_C->Color.red = (A_u_char)(FLOAT2FIX2((tmpValue.val.color.redF) * 255) >> 16);
				now_C->Color.green = (A_u_char)(FLOAT2FIX2((tmpValue.val.color.greenF) * 255) >> 16);
				now_C->Color.blue = (A_u_char)(FLOAT2FIX2((tmpValue.val.color.blueF) * 255) >> 16);
				//suites.ANSICallbacksSuite1()->sprintf(out_data->return_msg,"Color[%d,%d,%d,%d]", now_C->Color.alpha, now_C->Color.red, now_C->Color.green, now_C->Color.blue);
				suites.StreamSuite4()->AEGP_DisposeStreamValue(&tmpValue);
				suites.StreamSuite4()->AEGP_DisposeStream(tmpStreamH);
				NumCount++;

			}
			else if (ChkKey == key_LINK_L) {
				suites.StreamSuite4()->AEGP_GetNewEffectStreamByIndex(myAEGPID_L, tmpEffectRefH, 1, &tmpStreamH);
				suites.StreamSuite4()->AEGP_GetNewStreamValue(myAEGPID_L, tmpStreamH, AEGP_LTimeMode_CompTime, &current_time, false, &tmpValue);
				AEGP_CompH CompH = NULL;
				AEGP_LayerH Link_LayerH = NULL;
				AEGP_EffectRefH Link_tmpEffectRefH = NULL;
				A_long link_eff_index = 0;
				suites.LayerSuite7()->AEGP_GetLayerParentComp(LayerH, &CompH);
				if (tmpValue.val.layer_id != 0) {
					suites.LayerSuite7()->AEGP_GetLayerFromLayerID(CompH, tmpValue.val.layer_id, &Link_LayerH);
					suites.EffectSuite3()->AEGP_GetLayerNumEffects(Link_LayerH, &link_eff_index);
					for (A_long Link_index = 0; Link_index < link_eff_index; Link_index++) {
						suites.EffectSuite3()->AEGP_GetLayerEffectByIndex(myAEGPID_L, Link_LayerH, Link_index, &Link_tmpEffectRefH);
						suites.EffectSuite3()->AEGP_GetInstalledKeyFromLayerEffect(Link_tmpEffectRefH, &ChkKey);
						if (ChkKey == key_NUM_L) {
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
							suites.StreamSuite4()->AEGP_GetNewEffectStreamByIndex(myAEGPID_L, Link_tmpEffectRefH, 1, &tmpStreamH);
							suites.StreamSuite4()->AEGP_GetNewStreamValue(myAEGPID_L, tmpStreamH, AEGP_LTimeMode_CompTime, &current_time, false, &tmpValue);
							now_C->number = tmpValue.val.one_d;
							if (now_C->number > maxV) maxV = now_C->number;
							suites.StreamSuite4()->AEGP_DisposeStreamValue(&tmpValue);
							suites.StreamSuite4()->AEGP_DisposeStream(tmpStreamH);

							suites.StreamSuite4()->AEGP_GetNewEffectStreamByIndex(myAEGPID_L, Link_tmpEffectRefH, 2, &tmpStreamH);
							suites.StreamSuite4()->AEGP_GetNewStreamValue(myAEGPID_L, tmpStreamH, AEGP_LTimeMode_CompTime, &current_time, false, &tmpValue);
							now_C->Color.alpha = 255;
							now_C->Color.red = (A_u_char)(FLOAT2FIX2((tmpValue.val.color.redF) * 255) >> 16);
							now_C->Color.green = (A_u_char)(FLOAT2FIX2((tmpValue.val.color.greenF) * 255) >> 16);
							now_C->Color.blue = (A_u_char)(FLOAT2FIX2((tmpValue.val.color.blueF) * 255) >> 16);
							suites.StreamSuite4()->AEGP_DisposeStreamValue(&tmpValue);
							suites.StreamSuite4()->AEGP_DisposeStream(tmpStreamH);
							NumCount++;

						}
						suites.EffectSuite3()->AEGP_DisposeEffect(Link_tmpEffectRefH);
					}
				}
			}
			suites.EffectSuite3()->AEGP_DisposeEffect(tmpEffectRefH);

		}
	}


	/*RENDER!!!!!!!!!!!!!!!!*/

	//Create Dot
	PF_EffectWorld dot;
	Color_Feather dot_CF;
	dot_CF.Radius = (FLOAT2FIX2(params[DOT_WIDTH_FS]->u.fs_d.value / 2) >> 16) ;
	dot_CF.Start_Feather_Radius = ((FLOAT2FIX2(params[DOT_WIDTH_FS]->u.fs_d.value - (params[DOT_WIDTH_FS]->u.fs_d.value * params[DOT_FEATHER_FS]->u.fs_d.value / (PF_FpLong)100))) >> 16) / 2;
	if (params[USE_GCOLOR_CBOX]->u.bd.value && params[RENDOT_CB]->u.bd.value) {
		dot_CF.color = params[COLOR_C]->u.cd.value;
		suites.WorldSuite1()->new_world(in_data->effect_ref, dot_CF.Radius * 2 + 2, dot_CF.Radius * 2 + 2, PF_NewWorldFlag_NONE, &dot);
		suites.Iterate8Suite1()->iterate(in_data, 0, 0, NULL, NULL, &dot_CF, &Make_Dot_Pix, &dot);
	}
	else if(params[RENDOT_CB]->u.bd.value){
		suites.WorldSuite1()->new_world(in_data->effect_ref, dot_CF.Radius * 2 + 2, dot_CF.Radius * 2 + 2, PF_NewWorldFlag_NONE, &dot);
	}

	//Create Segment
	PF_EffectWorld segment;
	Color_Feather seg_CF;
	seg_CF.Radius = (FLOAT2FIX2(params[LINE_WIDTH_FS]->u.fs_d.value / 2) >> 16) ;
	seg_CF.Start_Feather_Radius = ((FLOAT2FIX2(params[LINE_WIDTH_FS]->u.fs_d.value - (params[LINE_WIDTH_FS]->u.fs_d.value * params[LINE_FEATHER_FS]->u.fs_d.value / (PF_FpLong)100))) >> 16) / 2;
	seg_CF.color = (params[LINE_COLOR_C]->u.cd.value);
	if (params[RENLINE_CB]->u.bd.value) {
		suites.WorldSuite1()->new_world(in_data->effect_ref, FLOAT2FIX2(params[SPACING_FS]->u.fs_d.value)>>16, seg_CF.Radius * 2 + 2, PF_NewWorldFlag_NONE, &segment);
		suites.Iterate8Suite1()->iterate(in_data, 0, 0, NULL, NULL, &seg_CF, &Make_Segment_Pix, &segment);
	}
	//Create Segment Dot
	PF_EffectWorld segment_dot;
	if (params[RENLINE_CB]->u.bd.value) {
		suites.WorldSuite1()->new_world(in_data->effect_ref, seg_CF.Radius * 2 + 2, seg_CF.Radius * 2 + 2, PF_NewWorldFlag_NONE, &segment_dot);
		suites.Iterate8Suite1()->iterate(in_data, 0, 0, NULL, NULL, &seg_CF, &Make_Dot_Pix, &segment_dot);
	}

	maxV = (maxV > params[STANDARD_FS]->u.fs_d.value) ? maxV : params[STANDARD_FS]->u.fs_d.value;

	//Create temp world

	PF_EffectWorld Tmp_dst;
	A_u_char width_determin = (params[DOT_WIDTH_FS]->u.fs_d.value > params[LINE_WIDTH_FS]->u.fs_d.value) ? 1 : 2;
	PF_FpLong width_determin_value = (width_determin == 1) ? params[DOT_WIDTH_FS]->u.fs_d.value + 2 : params[LINE_WIDTH_FS]->u.fs_d.value+2;
	suites.WorldSuite1()->new_world(in_data->effect_ref,FLOAT2FIX2(params[SPACING_FS]->u.fs_d.value*NumCount + width_determin_value)>>16 ,
		FLOAT2FIX2(params[MAXHEIGHT_FS]->u.fs_d.value + width_determin_value)>>16, PF_NewWorldFlag_CLEAR_PIXELS, &Tmp_dst);


	
	//Render Face First
	if (head != NULL) {
		if (params[RENFACE_CB]->u.bd.value && (params[FACE_OPACITY_FS]->u.fs_d.value != 0)) {
			Face_Struct tmp;
			tmp.color = params[FACE_COLOR_C]->u.cd.value;
			tmp.color.alpha = (A_u_char)(FLOAT2FIX2(255 * params[FACE_OPACITY_FS]->u.fs_d.value / 100) >> 16);
			tmp.Start_x = width_determin_value / 2;
			PF_FpLong End_y;
			PF_Rect Ren_area;
			Ren_area.bottom = FLOAT2FIX2(params[MAXHEIGHT_FS]->u.fs_d.value + width_determin_value / 2) >> 16;
			Ren_area.top = 0;
			Ren_area.left = FLOAT2FIX2(width_determin_value / 2) >> 16;
			Ren_area.right = Ren_area.left + (FLOAT2FIX2(params[SPACING_FS]->u.fs_d.value) >> 16);
			now = head;
			tmp.Start_y = params[MAXHEIGHT_FS]->u.fs_d.value - (now->number * params[MAXHEIGHT_FS]->u.fs_d.value / maxV) + width_determin_value / 2;
			for (; now != NULL; now = now->next, tmp.Start_x += params[SPACING_FS]->u.fs_d.value) {
				if (now->next != NULL) {
					End_y = params[MAXHEIGHT_FS]->u.fs_d.value - (now->next->number * params[MAXHEIGHT_FS]->u.fs_d.value / maxV) + width_determin_value / 2;
					tmp.slope = (End_y - tmp.Start_y) / params[SPACING_FS]->u.fs_d.value;
					suites.Iterate8Suite1()->iterate(in_data, 0, 0, NULL,&Ren_area, &tmp, &Make_Face_Pix, &Tmp_dst);
					Ren_area.left += FLOAT2FIX2(params[SPACING_FS]->u.fs_d.value) >> 16;
					Ren_area.right += FLOAT2FIX2(params[SPACING_FS]->u.fs_d.value) >> 16;
					tmp.Start_y = End_y;
				}
				else {
					break;
				}
			}
		}
	}
	else if (head_C != NULL) {
		if (params[RENFACE_CB]->u.bd.value && (params[FACE_OPACITY_FS]->u.fs_d.value != 0)) {
			Face_Struct tmp;
			tmp.color = params[FACE_COLOR_C]->u.cd.value;
			tmp.color.alpha = (A_u_char)(FLOAT2FIX2(255 * params[FACE_OPACITY_FS]->u.fs_d.value / 100) >> 16);
			tmp.Start_x = width_determin_value / 2;
			PF_FpLong End_y;
			PF_Rect Ren_area;
			Ren_area.bottom = FLOAT2FIX2(params[MAXHEIGHT_FS]->u.fs_d.value + width_determin_value / 2) >> 16;
			Ren_area.top = 0;
			Ren_area.left = FLOAT2FIX2(width_determin_value / 2) >> 16;
			Ren_area.right = Ren_area.left + (FLOAT2FIX2(params[SPACING_FS]->u.fs_d.value) >> 16);
			now_C = head_C;
			tmp.Start_y = params[MAXHEIGHT_FS]->u.fs_d.value - (now_C->number * params[MAXHEIGHT_FS]->u.fs_d.value / maxV) + width_determin_value / 2;
			for (; now_C != NULL; now_C = now_C->next, tmp.Start_x += params[SPACING_FS]->u.fs_d.value) {
				if (now_C->next != NULL) {
					End_y = params[MAXHEIGHT_FS]->u.fs_d.value - (now_C->next->number * params[MAXHEIGHT_FS]->u.fs_d.value / maxV) + width_determin_value / 2;
					tmp.slope = (End_y - tmp.Start_y) / params[SPACING_FS]->u.fs_d.value;
					suites.Iterate8Suite1()->iterate(in_data, 0, 0, NULL, &Ren_area, &tmp, &Make_Face_Pix, &Tmp_dst);
					Ren_area.left += FLOAT2FIX2(params[SPACING_FS]->u.fs_d.value) >> 16;
					Ren_area.right += FLOAT2FIX2(params[SPACING_FS]->u.fs_d.value) >> 16;
					tmp.Start_y = End_y;
				}
				else {
					break;
				}
			}
		}
	}




	//Render onto Tmp_dst
	PF_CompositeMode Normal_CmpMode = { PF_Xfer_IN_FRONT , 0, 255, false, 0 };
	PF_FpLong x = width_determin_value / 2, y;
	PF_FpLong dy;
	PF_FloatMatrix transFormMatrix_Seg, transFormMatrix_Dot, transFormMatrix_SegDot;
	if ((head != NULL) || (head_C != NULL)) {	//has something to render
		if (params[USE_GCOLOR_CBOX]->u.bd.value) { //Use global color
			for (now = head, head = NULL; now != NULL; now = now->next, x += params[SPACING_FS]->u.fs_d.value) {
				y = params[MAXHEIGHT_FS]->u.fs_d.value - (now->number * params[MAXHEIGHT_FS]->u.fs_d.value / maxV);
				//Render segments
				if (params[RENLINE_CB]->u.bd.value && params[LINE_WIDTH_FS]->u.fs_d.value != 0) {//Wants segment
					if (now->next != NULL) {	//Needs segment
						CCU_SetIdentityMatrix(&transFormMatrix_Seg);
						CCU_SetIdentityMatrix(&transFormMatrix_SegDot);
						dy = (now->next->number - now->number)*params[MAXHEIGHT_FS]->u.fs_d.value/maxV;

						CCU_ScaleMatrix(&transFormMatrix_Seg, 1 / suites.ANSICallbacksSuite1()->cos(suites.ANSICallbacksSuite1()->atan2(-dy, params[SPACING_FS]->u.fs_d.value)), 1, 0, 0);// sec()*length
						CCU_RotateMatrixPlus(&transFormMatrix_Seg, in_data, suites.ANSICallbacksSuite1()->atan2(-dy, params[SPACING_FS]->u.fs_d.value), 0, params[LINE_WIDTH_FS]->u.fs_d.value/2 + 1);
						if (width_determin == 1) {
							CCU_TransformPointsFloat(&transFormMatrix_Seg, x, y + width_determin_value/2 - params[LINE_WIDTH_FS]->u.fs_d.value/2 - 1);
							CCU_TransformPointsFloat(&transFormMatrix_SegDot, x - (params[LINE_WIDTH_FS]->u.fs_d.value + 2) / 2, y + width_determin_value / 2 - params[LINE_WIDTH_FS]->u.fs_d.value / 2 - 1);
						}
						else {
							CCU_TransformPointsFloat(&transFormMatrix_Seg, x, y);
							CCU_TransformPointsFloat(&transFormMatrix_SegDot, x - (params[LINE_WIDTH_FS]->u.fs_d.value + 2) / 2, y);
						}
						suites.WorldTransformSuite1()->transform_world(in_data->effect_ref, in_data->quality, PF_MF_Alpha_PREMUL, PF_Field_FRAME, &segment,
							&Normal_CmpMode, NULL, &transFormMatrix_Seg, 1, true, &Tmp_dst.extent_hint, &Tmp_dst);
						suites.WorldTransformSuite1()->transform_world(in_data->effect_ref, in_data->quality, PF_MF_Alpha_PREMUL, PF_Field_FRAME, &segment_dot,
							&Normal_CmpMode, NULL, &transFormMatrix_SegDot, 1, true, &Tmp_dst.extent_hint, &Tmp_dst);
					}
					else {
						CCU_SetIdentityMatrix(&transFormMatrix_SegDot);
						if (width_determin == 1) {
							CCU_TransformPointsFloat(&transFormMatrix_SegDot, x - (params[LINE_WIDTH_FS]->u.fs_d.value + 2) / 2, y + width_determin_value / 2 - params[LINE_WIDTH_FS]->u.fs_d.value / 2 - 1);
						}
						else {
							CCU_TransformPointsFloat(&transFormMatrix_SegDot, x - (params[LINE_WIDTH_FS]->u.fs_d.value + 2) / 2, y);
						}
						suites.WorldTransformSuite1()->transform_world(in_data->effect_ref, in_data->quality, PF_MF_Alpha_PREMUL, PF_Field_FRAME, &segment_dot,
							&Normal_CmpMode, NULL, &transFormMatrix_SegDot, 1, true, &Tmp_dst.extent_hint, &Tmp_dst);
					}
				}
				//Render Dots
				if (params[RENDOT_CB]->u.bd.value) {
					CCU_SetIdentityMatrix(&transFormMatrix_Dot);

					if (width_determin == 2) {
						CCU_TransformPointsFloat(&transFormMatrix_Dot, x - (params[DOT_WIDTH_FS]->u.fs_d.value + 2) / 2, y + width_determin_value/2 - params[DOT_WIDTH_FS]->u.fs_d.value/2 -1);
					}
					else {
						CCU_TransformPointsFloat(&transFormMatrix_Dot, x - (params[DOT_WIDTH_FS]->u.fs_d.value + 2) / 2, y);
					}


					suites.WorldTransformSuite1()->transform_world(in_data->effect_ref, in_data->quality, PF_MF_Alpha_PREMUL, PF_Field_FRAME, &dot,
						&Normal_CmpMode, NULL, &transFormMatrix_Dot, 1, true, &Tmp_dst.extent_hint, &Tmp_dst);
				}
				delete head;
				head = now;
			}//End for loop
		}
		else { //Use layer color(Need re-iterate dot every time)
			for (now_C = head_C, head_C = NULL; now_C != NULL; now_C = now_C->next, x += params[SPACING_FS]->u.fs_d.value) {
				y = params[MAXHEIGHT_FS]->u.fs_d.value - (now_C->number * params[MAXHEIGHT_FS]->u.fs_d.value / maxV);
				//Render segments
				if (params[RENLINE_CB]->u.bd.value && params[LINE_WIDTH_FS]->u.fs_d.value != 0) {//Wants segment
					if (now_C->next != NULL) {	//Needs segment
						CCU_SetIdentityMatrix(&transFormMatrix_Seg);
						CCU_SetIdentityMatrix(&transFormMatrix_SegDot);
						dy = (now_C->next->number - now_C->number)*params[MAXHEIGHT_FS]->u.fs_d.value / maxV;

						CCU_ScaleMatrix(&transFormMatrix_Seg, 1 / suites.ANSICallbacksSuite1()->cos(suites.ANSICallbacksSuite1()->atan2(-dy, params[SPACING_FS]->u.fs_d.value)), 1, 0, 0);// sec()*length
						CCU_RotateMatrixPlus(&transFormMatrix_Seg, in_data, suites.ANSICallbacksSuite1()->atan2(-dy, params[SPACING_FS]->u.fs_d.value), 0, params[LINE_WIDTH_FS]->u.fs_d.value / 2 + 1);

						if (width_determin == 1) {
							CCU_TransformPointsFloat(&transFormMatrix_Seg, x, y + width_determin_value / 2 - params[LINE_WIDTH_FS]->u.fs_d.value / 2 - 1);
							CCU_TransformPointsFloat(&transFormMatrix_SegDot, x - (params[LINE_WIDTH_FS]->u.fs_d.value + 2) / 2, y + width_determin_value / 2 - params[LINE_WIDTH_FS]->u.fs_d.value / 2 - 1);
						}
						else {
							CCU_TransformPointsFloat(&transFormMatrix_Seg, x, y);
							CCU_TransformPointsFloat(&transFormMatrix_SegDot, x - (params[LINE_WIDTH_FS]->u.fs_d.value + 2) / 2, y);
						}

						suites.WorldTransformSuite1()->transform_world(in_data->effect_ref, in_data->quality, PF_MF_Alpha_PREMUL, PF_Field_FRAME, &segment,
							&Normal_CmpMode, NULL, &transFormMatrix_Seg, 1, true, &Tmp_dst.extent_hint, &Tmp_dst);
						suites.WorldTransformSuite1()->transform_world(in_data->effect_ref, in_data->quality, PF_MF_Alpha_PREMUL, PF_Field_FRAME, &segment_dot,
							&Normal_CmpMode, NULL, &transFormMatrix_SegDot, 1, true, &Tmp_dst.extent_hint, &Tmp_dst);
					}
					else {
						CCU_SetIdentityMatrix(&transFormMatrix_SegDot);
						if (width_determin == 1) {
							CCU_TransformPointsFloat(&transFormMatrix_SegDot, x - (params[LINE_WIDTH_FS]->u.fs_d.value + 2) / 2, y + width_determin_value / 2 - params[LINE_WIDTH_FS]->u.fs_d.value / 2 - 1);
						}
						else {
							CCU_TransformPointsFloat(&transFormMatrix_SegDot, x - (params[LINE_WIDTH_FS]->u.fs_d.value + 2) / 2, y);
						}
						suites.WorldTransformSuite1()->transform_world(in_data->effect_ref, in_data->quality, PF_MF_Alpha_PREMUL, PF_Field_FRAME, &segment_dot,
							&Normal_CmpMode, NULL, &transFormMatrix_SegDot, 1, true, &Tmp_dst.extent_hint, &Tmp_dst);
					}
				}
				//Render Dots
				if (params[RENDOT_CB]->u.bd.value) {
					dot_CF.color = now_C->Color;
					suites.Iterate8Suite1()->iterate(in_data, 0, 0, NULL, NULL, &dot_CF, &Make_Dot_Pix, &dot);
					CCU_SetIdentityMatrix(&transFormMatrix_Dot);
					if (width_determin == 2) {
						CCU_TransformPointsFloat(&transFormMatrix_Dot, x - (params[DOT_WIDTH_FS]->u.fs_d.value + 2) / 2, y + width_determin_value / 2 - params[DOT_WIDTH_FS]->u.fs_d.value / 2 - 1);
					}
					else {
						CCU_TransformPointsFloat(&transFormMatrix_Dot, x - (params[DOT_WIDTH_FS]->u.fs_d.value + 2) / 2, y);
					}
					suites.WorldTransformSuite1()->transform_world(in_data->effect_ref, in_data->quality, PF_MF_Alpha_PREMUL, PF_Field_FRAME, &dot,
						&Normal_CmpMode, NULL, &transFormMatrix_Dot, 1, true, &Tmp_dst.extent_hint, &Tmp_dst);
				}
				delete head_C;
				head_C = now_C;
			}//End for loop
		}
	}

	if (params[RENLINE_CB]->u.bd.value) {
		suites.WorldSuite1()->dispose_world(in_data->effect_ref, &segment);
	}
	if (params[RENDOT_CB]->u.bd.value) {
		suites.WorldSuite1()->dispose_world(in_data->effect_ref, &dot);
	}

	PF_FloatMatrix transFormMatrix_Dst;
	CCU_SetIdentityMatrix(&transFormMatrix_Dst);
	CCU_ScaleMatrix(&transFormMatrix_Dst, (PF_FpLong)in_data->downsample_x.num / (PF_FpLong)in_data->downsample_x.den / (PF_FpLong)in_data->pixel_aspect_ratio.num * (PF_FpLong)in_data->pixel_aspect_ratio.den,
		(PF_FpLong)in_data->downsample_y.num / (PF_FpLong)in_data->downsample_y.den,
		0,0);
	CCU_TransformPointsFloat(&transFormMatrix_Dst, FIX_2_FLOAT(params[START_POINT]->u.td.x_value) - width_determin_value / 2 * (PF_FpLong)in_data->downsample_x.num / (PF_FpLong)in_data->downsample_x.den / (PF_FpLong)in_data->pixel_aspect_ratio.num * (PF_FpLong)in_data->pixel_aspect_ratio.den,
		FIX_2_FLOAT(params[START_POINT]->u.td.y_value) - (FIX_2_FLOAT(Tmp_dst.height<<16) - width_determin_value / 2)*(PF_FpLong)in_data->downsample_y.num / (PF_FpLong)in_data->downsample_y.den);
	suites.WorldTransformSuite1()->transform_world(in_data->effect_ref, in_data->quality, PF_MF_Alpha_PREMUL, PF_Field_FRAME, &Tmp_dst,
		&Normal_CmpMode, NULL, &transFormMatrix_Dst, 1, true, &output->extent_hint, output);

	suites.WorldSuite1()->dispose_world(in_data->effect_ref, &Tmp_dst);
	return err;
}

DllExport
PF_Err
EntryPointFuncL(
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

			err = About_L(in_data,
				out_data,
				params,
				output);
			break;

		case PF_Cmd_GLOBAL_SETUP:

			err = GlobalSetup_L(in_data,
				out_data,
				params,
				output);
			break;

		case PF_Cmd_PARAMS_SETUP:

			err = ParamsSetup_L(in_data,
				out_data,
				params,
				output);
			break;

		case PF_Cmd_RENDER:
			err = RenderLINE(in_data,
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


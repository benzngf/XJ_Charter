#ifdef __cplusplus
extern "C" {
#endif
	PF_Err
		CCU_ScaleMatrix(
			PF_FloatMatrix           *mP,
			PF_FpLong                     scaleX,
			PF_FpLong                     scaleY,
			PF_FpLong                     aboutX,
			PF_FpLong                     aboutY);

	PF_Err
		CCU_RotateMatrixPlus(
			PF_FloatMatrix          *matrixP,
			PF_InData                    *in_data,
			PF_FpLong                    radiansF,
			PF_FpLong                    aboutXF,
			PF_FpLong                    aboutYF);

	void CCU_SetIdentityMatrix(
		PF_FloatMatrix *matrixP);

	PF_Err
		CCU_TransformPoints(
			PF_FloatMatrix          *matrixP,
			const PF_Point          *point);

	PF_Err
		CCU_TransformPointsFloat(
			PF_FloatMatrix          *matrixP,
			const PF_FpLong          h, const PF_FpLong          v);
#ifdef __cplusplus
}
#endif
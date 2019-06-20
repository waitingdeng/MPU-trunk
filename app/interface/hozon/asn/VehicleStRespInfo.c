/*
 * Generated by asn1c-0.9.28 (http://lionet.info/asn1c)
 * From ASN.1 module "HOZON"
 * 	found in "HOZON_PRIV_v1.0.asn"
 * 	`asn1c -gen-PER`
 */

#include "VehicleStRespInfo.h"

static int
memb_statusTime_constraint_1(asn_TYPE_descriptor_t *td, const void *sptr,
			asn_app_constraint_failed_f *ctfailcb, void *app_key) {
	long value;
	
	if(!sptr) {
		ASN__CTFAIL(app_key, td, sptr,
			"%s: value not given (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}
	
	value = *(const long *)sptr;
	
	if((value >= 0 && value <= 2147483647)) {
		/* Constraint check succeeded */
		return 0;
	} else {
		ASN__CTFAIL(app_key, td, sptr,
			"%s: constraint failed (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}
}

static int
memb_vsgpsPos_constraint_1(asn_TYPE_descriptor_t *td, const void *sptr,
			asn_app_constraint_failed_f *ctfailcb, void *app_key) {
	size_t size;
	
	if(!sptr) {
		ASN__CTFAIL(app_key, td, sptr,
			"%s: value not given (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}
	
	/* Determine the number of elements */
	size = _A_CSEQUENCE_FROM_VOID(sptr)->count;
	
	if((size == 1)) {
		/* Perform validation of the inner elements */
		return td->check_constraints(td, sptr, ctfailcb, app_key);
	} else {
		ASN__CTFAIL(app_key, td, sptr,
			"%s: constraint failed (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}
}

static int
memb_vehiclebasicSt_constraint_1(asn_TYPE_descriptor_t *td, const void *sptr,
			asn_app_constraint_failed_f *ctfailcb, void *app_key) {
	size_t size;
	
	if(!sptr) {
		ASN__CTFAIL(app_key, td, sptr,
			"%s: value not given (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}
	
	/* Determine the number of elements */
	size = _A_CSEQUENCE_FROM_VOID(sptr)->count;
	
	if((size == 1)) {
		/* Perform validation of the inner elements */
		return td->check_constraints(td, sptr, ctfailcb, app_key);
	} else {
		ASN__CTFAIL(app_key, td, sptr,
			"%s: constraint failed (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}
}

static int
memb_exVehicleSt_constraint_1(asn_TYPE_descriptor_t *td, const void *sptr,
			asn_app_constraint_failed_f *ctfailcb, void *app_key) {
	size_t size;
	
	if(!sptr) {
		ASN__CTFAIL(app_key, td, sptr,
			"%s: value not given (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}
	
	/* Determine the number of elements */
	size = _A_CSEQUENCE_FROM_VOID(sptr)->count;
	
	if((size == 1)) {
		/* Perform validation of the inner elements */
		return td->check_constraints(td, sptr, ctfailcb, app_key);
	} else {
		ASN__CTFAIL(app_key, td, sptr,
			"%s: constraint failed (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}
}

static asn_per_constraints_t asn_PER_type_vsgpsPos_constr_3 GCC_NOTUSED = {
	{ APC_UNCONSTRAINED,	-1, -1,  0,  0 },
	{ APC_CONSTRAINED,	 0,  0,  1,  1 }	/* (SIZE(1..1)) */,
	0, 0	/* No PER value map */
};
static asn_per_constraints_t asn_PER_type_vehiclebasicSt_constr_5 GCC_NOTUSED = {
	{ APC_UNCONSTRAINED,	-1, -1,  0,  0 },
	{ APC_CONSTRAINED,	 0,  0,  1,  1 }	/* (SIZE(1..1)) */,
	0, 0	/* No PER value map */
};
static asn_per_constraints_t asn_PER_type_exVehicleSt_constr_7 GCC_NOTUSED = {
	{ APC_UNCONSTRAINED,	-1, -1,  0,  0 },
	{ APC_CONSTRAINED,	 0,  0,  1,  1 }	/* (SIZE(1..1)) */,
	0, 0	/* No PER value map */
};
static asn_per_constraints_t asn_PER_memb_statusTime_constr_2 GCC_NOTUSED = {
	{ APC_CONSTRAINED,	 31, -1,  0,  2147483647 }	/* (0..2147483647) */,
	{ APC_UNCONSTRAINED,	-1, -1,  0,  0 },
	0, 0	/* No PER value map */
};
static asn_per_constraints_t asn_PER_memb_vsgpsPos_constr_3 GCC_NOTUSED = {
	{ APC_UNCONSTRAINED,	-1, -1,  0,  0 },
	{ APC_CONSTRAINED,	 0,  0,  1,  1 }	/* (SIZE(1..1)) */,
	0, 0	/* No PER value map */
};
static asn_per_constraints_t asn_PER_memb_vehiclebasicSt_constr_5 GCC_NOTUSED = {
	{ APC_UNCONSTRAINED,	-1, -1,  0,  0 },
	{ APC_CONSTRAINED,	 0,  0,  1,  1 }	/* (SIZE(1..1)) */,
	0, 0	/* No PER value map */
};
static asn_per_constraints_t asn_PER_memb_exVehicleSt_constr_7 GCC_NOTUSED = {
	{ APC_UNCONSTRAINED,	-1, -1,  0,  0 },
	{ APC_CONSTRAINED,	 0,  0,  1,  1 }	/* (SIZE(1..1)) */,
	0, 0	/* No PER value map */
};
static asn_TYPE_member_t asn_MBR_vsgpsPos_3[] = {
	{ ATF_POINTER, 0, 0,
		(ASN_TAG_CLASS_UNIVERSAL | (16 << 2)),
		0,
		&asn_DEF_VSgpspos,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		""
		},
};
static const ber_tlv_tag_t asn_DEF_vsgpsPos_tags_3[] = {
	(ASN_TAG_CLASS_CONTEXT | (1 << 2)),
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static asn_SET_OF_specifics_t asn_SPC_vsgpsPos_specs_3 = {
	sizeof(struct vsgpsPos),
	offsetof(struct vsgpsPos, _asn_ctx),
	0,	/* XER encoding is XMLDelimitedItemList */
};
static /* Use -fall-defs-global to expose */
asn_TYPE_descriptor_t asn_DEF_vsgpsPos_3 = {
	"vsgpsPos",
	"vsgpsPos",
	SEQUENCE_OF_free,
	SEQUENCE_OF_print,
	SEQUENCE_OF_constraint,
	SEQUENCE_OF_decode_ber,
	SEQUENCE_OF_encode_der,
	SEQUENCE_OF_decode_xer,
	SEQUENCE_OF_encode_xer,
	SEQUENCE_OF_decode_uper,
	SEQUENCE_OF_encode_uper,
	0,	/* Use generic outmost tag fetcher */
	asn_DEF_vsgpsPos_tags_3,
	sizeof(asn_DEF_vsgpsPos_tags_3)
		/sizeof(asn_DEF_vsgpsPos_tags_3[0]) - 1, /* 1 */
	asn_DEF_vsgpsPos_tags_3,	/* Same as above */
	sizeof(asn_DEF_vsgpsPos_tags_3)
		/sizeof(asn_DEF_vsgpsPos_tags_3[0]), /* 2 */
	&asn_PER_type_vsgpsPos_constr_3,
	asn_MBR_vsgpsPos_3,
	1,	/* Single element */
	&asn_SPC_vsgpsPos_specs_3	/* Additional specs */
};

static asn_TYPE_member_t asn_MBR_vehiclebasicSt_5[] = {
	{ ATF_POINTER, 0, 0,
		(ASN_TAG_CLASS_UNIVERSAL | (16 << 2)),
		0,
		&asn_DEF_RvsBasicStatus,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		""
		},
};
static const ber_tlv_tag_t asn_DEF_vehiclebasicSt_tags_5[] = {
	(ASN_TAG_CLASS_CONTEXT | (2 << 2)),
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static asn_SET_OF_specifics_t asn_SPC_vehiclebasicSt_specs_5 = {
	sizeof(struct vehiclebasicSt),
	offsetof(struct vehiclebasicSt, _asn_ctx),
	0,	/* XER encoding is XMLDelimitedItemList */
};
static /* Use -fall-defs-global to expose */
asn_TYPE_descriptor_t asn_DEF_vehiclebasicSt_5 = {
	"vehiclebasicSt",
	"vehiclebasicSt",
	SEQUENCE_OF_free,
	SEQUENCE_OF_print,
	SEQUENCE_OF_constraint,
	SEQUENCE_OF_decode_ber,
	SEQUENCE_OF_encode_der,
	SEQUENCE_OF_decode_xer,
	SEQUENCE_OF_encode_xer,
	SEQUENCE_OF_decode_uper,
	SEQUENCE_OF_encode_uper,
	0,	/* Use generic outmost tag fetcher */
	asn_DEF_vehiclebasicSt_tags_5,
	sizeof(asn_DEF_vehiclebasicSt_tags_5)
		/sizeof(asn_DEF_vehiclebasicSt_tags_5[0]) - 1, /* 1 */
	asn_DEF_vehiclebasicSt_tags_5,	/* Same as above */
	sizeof(asn_DEF_vehiclebasicSt_tags_5)
		/sizeof(asn_DEF_vehiclebasicSt_tags_5[0]), /* 2 */
	&asn_PER_type_vehiclebasicSt_constr_5,
	asn_MBR_vehiclebasicSt_5,
	1,	/* Single element */
	&asn_SPC_vehiclebasicSt_specs_5	/* Additional specs */
};

static asn_TYPE_member_t asn_MBR_exVehicleSt_7[] = {
	{ ATF_POINTER, 0, 0,
		(ASN_TAG_CLASS_UNIVERSAL | (16 << 2)),
		0,
		&asn_DEF_VSExtStatus,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		""
		},
};
static const ber_tlv_tag_t asn_DEF_exVehicleSt_tags_7[] = {
	(ASN_TAG_CLASS_CONTEXT | (3 << 2)),
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static asn_SET_OF_specifics_t asn_SPC_exVehicleSt_specs_7 = {
	sizeof(struct exVehicleSt),
	offsetof(struct exVehicleSt, _asn_ctx),
	0,	/* XER encoding is XMLDelimitedItemList */
};
static /* Use -fall-defs-global to expose */
asn_TYPE_descriptor_t asn_DEF_exVehicleSt_7 = {
	"exVehicleSt",
	"exVehicleSt",
	SEQUENCE_OF_free,
	SEQUENCE_OF_print,
	SEQUENCE_OF_constraint,
	SEQUENCE_OF_decode_ber,
	SEQUENCE_OF_encode_der,
	SEQUENCE_OF_decode_xer,
	SEQUENCE_OF_encode_xer,
	SEQUENCE_OF_decode_uper,
	SEQUENCE_OF_encode_uper,
	0,	/* Use generic outmost tag fetcher */
	asn_DEF_exVehicleSt_tags_7,
	sizeof(asn_DEF_exVehicleSt_tags_7)
		/sizeof(asn_DEF_exVehicleSt_tags_7[0]) - 1, /* 1 */
	asn_DEF_exVehicleSt_tags_7,	/* Same as above */
	sizeof(asn_DEF_exVehicleSt_tags_7)
		/sizeof(asn_DEF_exVehicleSt_tags_7[0]), /* 2 */
	&asn_PER_type_exVehicleSt_constr_7,
	asn_MBR_exVehicleSt_7,
	1,	/* Single element */
	&asn_SPC_exVehicleSt_specs_7	/* Additional specs */
};

static asn_TYPE_member_t asn_MBR_VehicleStRespInfo_1[] = {
	{ ATF_NOFLAGS, 0, offsetof(struct VehicleStRespInfo, statusTime),
		(ASN_TAG_CLASS_CONTEXT | (0 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		memb_statusTime_constraint_1,
		&asn_PER_memb_statusTime_constr_2,
		0,
		"statusTime"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct VehicleStRespInfo, vsgpsPos),
		(ASN_TAG_CLASS_CONTEXT | (1 << 2)),
		0,
		&asn_DEF_vsgpsPos_3,
		memb_vsgpsPos_constraint_1,
		&asn_PER_memb_vsgpsPos_constr_3,
		0,
		"vsgpsPos"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct VehicleStRespInfo, vehiclebasicSt),
		(ASN_TAG_CLASS_CONTEXT | (2 << 2)),
		0,
		&asn_DEF_vehiclebasicSt_5,
		memb_vehiclebasicSt_constraint_1,
		&asn_PER_memb_vehiclebasicSt_constr_5,
		0,
		"vehiclebasicSt"
		},
	{ ATF_POINTER, 1, offsetof(struct VehicleStRespInfo, exVehicleSt),
		(ASN_TAG_CLASS_CONTEXT | (3 << 2)),
		0,
		&asn_DEF_exVehicleSt_7,
		memb_exVehicleSt_constraint_1,
		&asn_PER_memb_exVehicleSt_constr_7,
		0,
		"exVehicleSt"
		},
};
static const int asn_MAP_VehicleStRespInfo_oms_1[] = { 3 };
static const ber_tlv_tag_t asn_DEF_VehicleStRespInfo_tags_1[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static const asn_TYPE_tag2member_t asn_MAP_VehicleStRespInfo_tag2el_1[] = {
    { (ASN_TAG_CLASS_CONTEXT | (0 << 2)), 0, 0, 0 }, /* statusTime */
    { (ASN_TAG_CLASS_CONTEXT | (1 << 2)), 1, 0, 0 }, /* vsgpsPos */
    { (ASN_TAG_CLASS_CONTEXT | (2 << 2)), 2, 0, 0 }, /* vehiclebasicSt */
    { (ASN_TAG_CLASS_CONTEXT | (3 << 2)), 3, 0, 0 } /* exVehicleSt */
};
static asn_SEQUENCE_specifics_t asn_SPC_VehicleStRespInfo_specs_1 = {
	sizeof(struct VehicleStRespInfo),
	offsetof(struct VehicleStRespInfo, _asn_ctx),
	asn_MAP_VehicleStRespInfo_tag2el_1,
	4,	/* Count of tags in the map */
	asn_MAP_VehicleStRespInfo_oms_1,	/* Optional members */
	1, 0,	/* Root/Additions */
	-1,	/* Start extensions */
	-1	/* Stop extensions */
};
asn_TYPE_descriptor_t asn_DEF_VehicleStRespInfo = {
	"VehicleStRespInfo",
	"VehicleStRespInfo",
	SEQUENCE_free,
	SEQUENCE_print,
	SEQUENCE_constraint,
	SEQUENCE_decode_ber,
	SEQUENCE_encode_der,
	SEQUENCE_decode_xer,
	SEQUENCE_encode_xer,
	SEQUENCE_decode_uper,
	SEQUENCE_encode_uper,
	0,	/* Use generic outmost tag fetcher */
	asn_DEF_VehicleStRespInfo_tags_1,
	sizeof(asn_DEF_VehicleStRespInfo_tags_1)
		/sizeof(asn_DEF_VehicleStRespInfo_tags_1[0]), /* 1 */
	asn_DEF_VehicleStRespInfo_tags_1,	/* Same as above */
	sizeof(asn_DEF_VehicleStRespInfo_tags_1)
		/sizeof(asn_DEF_VehicleStRespInfo_tags_1[0]), /* 1 */
	0,	/* No PER visible constraints */
	asn_MBR_VehicleStRespInfo_1,
	4,	/* Elements count */
	&asn_SPC_VehicleStRespInfo_specs_1	/* Additional specs */
};

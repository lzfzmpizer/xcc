/**********************************************************************************************************/
/*                                                                                                        */
/*                                Copyright (c) 2021-2022 by Pizer.Fan                                    */
/*                                                                                                        */
/*  This software is copyrighted by and is the sole property of Pizer.Fan. All rights, title, ownership,  */
/*  or other interests in the software remain the property of Pizer.Fan. Any unauthorized use,            */
/*  duplication, transmission, distribution, or disclosure of this software is expressly forbidden.       */
/*  This Copyright notice may not be removed or modified without prior written consent of Pizer.Fan       */
/*                                                                                                        */
/**********************************************************************************************************/

/**********************************************************************************************************/
/*  Love Operating System                                                                                 */
/*                                                                                                        */
/*    Logging Subsystem                                                                                   */
/*                                                                                                        */
/**********************************************************************************************************/

/**********************************************************************************************************/
/*  Release History                                                                                       */
/*                                                                                                        */
/*    Date              Name                      Description                                             */
/*                                                                                                        */
/*  2022/08/20      Pizer.Fan                   Initial Version                                           */
/*                                                                                                        */
/**********************************************************************************************************/

#ifndef __XCC_LS_LOG_H__
#define __XCC_LS_LOG_H__

#ifdef __cplusplus
extern "C"
{
#endif

/*========================================================================================================*/
/* config static level: common level, print level, func call level                                        */
/*   NOTE: print level and func call level SHALL be not greater than common level:                        */
/*         LS_LOG_PRINT_LEVEL <= LS_LOG_LEVEL                                                             */
/*========================================================================================================*/
#if !defined(LS_LOG_LEVEL)
#define LS_LOG_LEVEL                       LS_LOG_LEVEL_DEBUG
#endif /* #if !defined(LS_LOG_LEVEL) */

/*========================================================================================================*/
/* Log level                                                                                              */
/*========================================================================================================*/
#define LS_LOG_LEVEL_CRIT           0
#define LS_LOG_LEVEL_WARNING        1
#define LS_LOG_LEVEL_INFO           2
#define LS_LOG_LEVEL_DEBUG          3

/*========================================================================================================*/
/* Variables declaration                                                                                  */
/*========================================================================================================*/

extern int ls_log_curr_level_g;   /*!< the current log level (running time) */

/*========================================================================================================*/
/* Filters                                                                                                */
/*========================================================================================================*/

#define LS_LOG_IS_NOT_FILTERED_OUT_LEVEL(_level) ((int)(LS_LOG_LEVEL_##_level) <= ls_log_curr_level_g)
#define LS_LOG_IS_NOT_FILTERED_OUT_MOD_LEVEL(_mod, _level) LS_TRUE

/*========================================================================================================*/
/* API: LS_LOG(_level, _mod, _fmt, ...)                                                                   */
/*   @param _level CRIT,WARNING,INFO,DEBUG                                                                */
/*   @param _mod the token of module name, such as FE, FTL_GC, BE, ...                                    */
/*   @param _fmt the format string to be log.                                                             */
/*   @param ...                                                                                           */
/* For example:                                                                                           */
/*   LS_LOG(CRIT , FE, "test");                                                                           */
/*   LS_LOG(DEBUG, FE_READ_CMD, "test: %d", var);                                                         */
/*========================================================================================================*/

#define LS_LOG(_level, _mod, _fmt, ...) \
  LS_LOG_##_level(_mod, _fmt, ##__VA_ARGS__)

#if (LS_LOG_LEVEL_CRIT <= LS_LOG_LEVEL)
#define LS_LOG_CRIT(_mod, _fmt, ...) LS_LOG_INVOKE(CRIT, _mod, _fmt, ##__VA_ARGS__)
#else
#define LS_LOG_CRIT(_mod, _fmt, ...)
#endif
#if (LS_LOG_LEVEL_WARNING <= LS_LOG_LEVEL)
#define LS_LOG_WARNING(_mod, _fmt, ...) LS_LOG_INVOKE(WARNING, _mod, _fmt, ##__VA_ARGS__)
#else
#define LS_LOG_WARNING(_mod, _fmt, ...)
#endif
#if (LS_LOG_LEVEL_INFO <= LS_LOG_LEVEL)
#define LS_LOG_INFO(_mod, _fmt, ...) LS_LOG_INVOKE(INFO, _mod, _fmt, ##__VA_ARGS__)
#else
#define LS_LOG_INFO(_mod, _fmt, ...)
#endif
#if (LS_LOG_LEVEL_DEBUG <= LS_LOG_LEVEL)
#define LS_LOG_DEBUG(_mod, _fmt, ...) LS_LOG_INVOKE(DEBUG, _mod, _fmt, ##__VA_ARGS__)
#else
#define LS_LOG_DEBUG(_mod, _fmt, ...)
#endif

#if defined(LS_LOG_DYNMICAL_FILTER_SUPPORTED) && (LS_LOG_DYNMICAL_FILTER_SUPPORTED)
  #define LS_LOG_INVOKE(_level, _mod, _fmt, ...) \
    do \
    { \
      if(LS_LOG_IS_NOT_FILTERED_OUT_LEVEL(_level) && LS_LOG_IS_NOT_FILTERED_OUT_MOD_LEVEL(_mod, _level)) \
      { \
        ls_print(__FUNCTION__, sizeof(__FUNCTION__)-1, __LINE__, LS_LOG_LEVEL_##_level, LS_LOG_MOD_##_mod, _fmt, ##__VA_ARGS__); \
      } \
    } while(0)
#else
  #define LS_LOG_INVOKE(_level, _mod, _fmt, ...) \
    ls_print(__FUNCTION__, sizeof(__FUNCTION__)-1, __LINE__, LS_LOG_LEVEL_##_level, LS_LOG_MOD_##_mod, _fmt, ##__VA_ARGS__)
#endif /* #if defined(LS_LOG_DYNMICAL_FILTER_SUPPORTED) && (LS_LOG_DYNMICAL_FILTER_SUPPORTED) */


/*========================================================================================================*/
/* LS_LOG_ID(_level, _mod, _fmt, _param_num, _params, _id)                                                */
/* NOTE: it's not API for user                                                                            */
/*   @param _level CRIT,WARNING,INFO,DEBUG                                                                */
/*   @param _mod the token of module name, such as FE, FTL_GC, BE, ...                                    */
/*   @param _fmt the format string, but it's useless for binary encoding.                                 */
/*   @param _param_num the parameter number                                                               */
/*   @param _params the parameters                                                                        */
/*   @param _id the trace id                                                                              */
/*========================================================================================================*/
 
#define LS_LOG_ID(_level, _mod, _fmt, _param_num, _params, _id) \
    LS_LOG_ID_##_level(_mod, _fmt, _param_num, _params, _id)

#if (LS_LOG_LEVEL_CRIT <= LS_LOG_LEVEL)
#define LS_LOG_ID_CRIT(_mod, _fmt, _param_num, _params, _id) LS_LOG_ID_INVOKE(CRIT, _mod, _fmt, _param_num, _params, _id)
#else
#define LS_LOG_ID_CRIT(_mod, _fmt, _param_num, _params, _id)
#endif
#if (LS_LOG_LEVEL_WARNING <= LS_LOG_LEVEL)
#define LS_LOG_ID_WARNING(_mod, _fmt, _param_num, _params, _id) LS_LOG_ID_INVOKE(WARNING, _mod, _fmt, _param_num, _params, _id)
#else
#define LS_LOG_ID_WARNING(_mod, _fmt, _param_num, _params, _id)
#endif
#if (LS_LOG_LEVEL_INFO <= LS_LOG_LEVEL)
#define LS_LOG_ID_INFO(_mod, _fmt, _param_num, _params, _id) LS_LOG_ID_INVOKE(INFO, _mod, _fmt, _param_num, _params, _id)
#else
#define LS_LOG_ID_INFO(_mod, _fmt, _param_num, _params, _id)
#endif
#if (LS_LOG_LEVEL_DEBUG <= LS_LOG_LEVEL)
#define LS_LOG_ID_DEBUG(_mod, _fmt, _param_num, _params, _id) LS_LOG_ID_INVOKE(DEBUG, _mod, _fmt, _param_num, _params, _id)
#else
#define LS_LOG_ID_DEBUG(_mod, _fmt, _param_num, _params, _id)
#endif

#if defined(LS_LOG_ID_SUPPORTED) && (LS_LOG_ID_SUPPORTED)
#if defined(LS_LOG_DYNMICAL_FILTER_SUPPORTED) && (LS_LOG_DYNMICAL_FILTER_SUPPORTED)
  #define LS_LOG_ID_INVOKE(_level, _mod, _fmt, _param_num, _params, _id) \
    do \
    { \
      if(LS_LOG_IS_NOT_FILTERED_OUT_LEVEL(_level) && LS_LOG_IS_NOT_FILTERED_OUT_MOD_LEVEL(_mod, _level)) \
      { \
        LS_LOG_ID_INVOKE2(_level, _mod, _fmt, _param_num, _params, _id)
      } \
    } while(0)
#else
  #define LS_LOG_ID_INVOKE(_level, _mod, _fmt, _param_num, _params, _id) \
    LS_LOG_ID_INVOKE2(_level, _mod, _fmt, _param_num, _params, _id)
#endif /* #if defined(LS_LOG_DYNMICAL_FILTER_SUPPORTED) && (LS_LOG_DYNMICAL_FILTER_SUPPORTED) */

#if defined(LS_LOG_ALWAYS_HAS_BUFFER) && (LS_LOG_ALWAYS_HAS_BUFFER)
  #define LS_LOG_ID_INVOKE2(_level, _mod, _fmt, _param_num, _params, _id) \
    LS_LOG_ID_INVOKE2_##_param_num (_level, _mod, _fmt, _param_num, _params, _id)
  #define LS_LOG_ID_INVOKE2_0(_level, _mod, _fmt, _param_num, _params, _id) LS_LOG_ID_INVOKE3(_level, _mod, _fmt, _param_num, _params, _id)
  #define LS_LOG_ID_INVOKE2_1(_level, _mod, _fmt, _param_num, _params, _id) LS_LOG_ID_INVOKE3(_level, _mod, _fmt, _param_num, _params, _id)
  #define LS_LOG_ID_INVOKE2_2(_level, _mod, _fmt, _param_num, _params, _id) LS_LOG_ID_INVOKE3(_level, _mod, _fmt, _param_num, _params, _id)
  #define LS_LOG_ID_INVOKE2_3(_level, _mod, _fmt, _param_num, _params, _id) LS_LOG_ID_INVOKE3(_level, _mod, _fmt, _param_num, _params, _id)
  #define LS_LOG_ID_INVOKE2_4(_level, _mod, _fmt, _param_num, _params, _id) LS_LOG_ID_INVOKE3(_level, _mod, _fmt, _param_num, _params, _id)
  #define LS_LOG_ID_INVOKE2_5(_level, _mod, _fmt, _param_num, _params, _id) LS_LOG_ID_INVOKE3(_level, _mod, _fmt, _param_num, _params, _id)
  #define LS_LOG_ID_INVOKE2_6(_level, _mod, _fmt, _param_num, _params, _id) LS_LOG_ID_INVOKE3(_level, _mod, _fmt, _param_num, _params, _id)
  #define LS_LOG_ID_INVOKE2_7(_level, _mod, _fmt, _param_num, _params, _id) LS_LOG_ID_INVOKE3(_level, _mod, _fmt, _param_num, _params, _id)
  #define LS_LOG_ID_INVOKE2_8(_level, _mod, _fmt, _param_num, _params, _id) LS_LOG_ID_INVOKE3(_level, _mod, _fmt, _param_num, _params, _id)
  #define LS_LOG_ID_INVOKE2_9(_level, _mod, _fmt, _param_num, _params, _id) LS_LOG_ID_INVOKE3(_level, _mod, _fmt, _param_num, _params, _id)
  #define LS_LOG_ID_INVOKE2_10(_level, _mod, _fmt, _param_num, _params, _id) LS_LOG_ID_INVOKE3(_level, _mod, _fmt, _param_num, _params, _id)
  #define LS_LOG_ID_INVOKE2_11(_level, _mod, _fmt, _param_num, _params, _id) LS_LOG_ID_INVOKE3(_level, _mod, _fmt, _param_num, _params, _id)
  #define LS_LOG_ID_INVOKE2_12(_level, _mod, _fmt, _param_num, _params, _id) LS_LOG_ID_INVOKE3(_level, _mod, _fmt, _param_num, _params, _id)
  #define LS_LOG_ID_INVOKE2_13(_level, _mod, _fmt, _param_num, _params, _id) LS_LOG_ID_INVOKE3(_level, _mod, _fmt, _param_num, _params, _id)
  #define LS_LOG_ID_INVOKE2_14(_level, _mod, _fmt, _param_num, _params, _id) LS_LOG_ID_INVOKE3(_level, _mod, _fmt, _param_num, _params, _id)
  #define LS_LOG_ID_INVOKE2_15(_level, _mod, _fmt, _param_num, _params, _id) LS_LOG_ID_INVOKE3(_level, _mod, _fmt, _param_num, _params, _id)
  #define LS_LOG_ID_INVOKE2_16(_level, _mod, _fmt, _param_num, _params, _id) LS_LOG_ID_INVOKE3(_level, _mod, _fmt, _param_num, _params, _id)
  #define LS_LOG_ID_INVOKE2_17(_level, _mod, _fmt, _param_num, _params, _id) LS_LOG_ID_INVOKE3(_level, _mod, _fmt, _param_num, _params, _id)
  #define LS_LOG_ID_INVOKE2_18(_level, _mod, _fmt, _param_num, _params, _id) LS_LOG_ID_INVOKE3(_level, _mod, _fmt, _param_num, _params, _id)
  #define LS_LOG_ID_INVOKE2_19(_level, _mod, _fmt, _param_num, _params, _id) LS_LOG_ID_INVOKE3(_level, _mod, _fmt, _param_num, _params, _id)
  #define LS_LOG_ID_INVOKE2_20(_level, _mod, _fmt, _param_num, _params, _id) LS_LOG_ID_INVOKE3(_level, _mod, _fmt, _param_num, _params, _id)
  #define LS_LOG_ID_INVOKE2_21(_level, _mod, _fmt, _param_num, _params, _id) LS_LOG_ID_INVOKE3(_level, _mod, _fmt, _param_num, _params, _id)
  #define LS_LOG_ID_INVOKE2_22(_level, _mod, _fmt, _param_num, _params, _id) LS_LOG_ID_INVOKE3(_level, _mod, _fmt, _param_num, _params, _id)
  #define LS_LOG_ID_INVOKE2_23(_level, _mod, _fmt, _param_num, _params, _id) LS_LOG_ID_INVOKE3(_level, _mod, _fmt, _param_num, _params, _id)
  #define LS_LOG_ID_INVOKE2_24(_level, _mod, _fmt, _param_num, _params, _id) LS_LOG_ID_INVOKE3(_level, _mod, _fmt, _param_num, _params, _id)
  #define LS_LOG_ID_INVOKE2_25(_level, _mod, _fmt, _param_num, _params, _id) LS_LOG_ID_INVOKE3(_level, _mod, _fmt, _param_num, _params, _id)
  #define LS_LOG_ID_INVOKE2_26(_level, _mod, _fmt, _param_num, _params, _id) LS_LOG_ID_INVOKE3(_level, _mod, _fmt, _param_num, _params, _id)
  #define LS_LOG_ID_INVOKE2_27(_level, _mod, _fmt, _param_num, _params, _id) LS_LOG_ID_INVOKE3(_level, _mod, _fmt, _param_num, _params, _id)
  #define LS_LOG_ID_INVOKE2_28(_level, _mod, _fmt, _param_num, _params, _id) LS_LOG_ID_INVOKE3(_level, _mod, _fmt, _param_num, _params, _id)
  #define LS_LOG_ID_INVOKE2_29(_level, _mod, _fmt, _param_num, _params, _id) LS_LOG_ID_INVOKE3(_level, _mod, _fmt, _param_num, _params, _id)
  #define LS_LOG_ID_INVOKE2_30(_level, _mod, _fmt, _param_num, _params, _id) LS_LOG_ID_INVOKE3(_level, _mod, _fmt, _param_num, _params, _id)
  #define LS_LOG_ID_INVOKE2_31(_level, _mod, _fmt, _param_num, _params, _id) LS_LOG_ID_INVOKE3(_level, _mod, _fmt, _param_num, _params, _id)
  #define LS_LOG_ID_INVOKE3(_level, _mod, _fmt, _param_num, _params, _id) \
    ls_log_id(LS_LOG_ENC_ID(_mod, _id, _param_num), _param_num LS_LOG_APPEND_ID_PARAMS_INVOKE(_param_num, _params))
#else
  #define LS_LOG_ID_INVOKE2(_level, _mod, _fmt, _param_num, _params, _id) \
    ls_log_id(LS_LOG_ENC_ID(_mod, _id, _param_num), _param_num LS_LOG_APPEND_ID_PARAMS_INVOKE(_param_num, _params))
#endif

#else
#if defined(LS_LOG_DYNMICAL_FILTER_SUPPORTED) && (LS_LOG_DYNMICAL_FILTER_SUPPORTED)
  #define LS_LOG_ID_INVOKE(_level, _mod, _fmt, _param_num, _params, _id) \
    do \
    { \
      if(LS_LOG_IS_NOT_FILTERED_OUT_LEVEL(_level) && LS_LOG_IS_NOT_FILTERED_OUT_MOD_LEVEL(_mod, _level)) \
      { \
        ls_print(__FUNCTION__, sizeof(__FUNCTION__)-1, __LINE__, LS_LOG_LEVEL_##_level, LS_LOG_MOD_##_mod, _fmt LS_LOG_APPEND_ID_PARAMS_INVOKE(_param_num, _params)); \
      } \
    } while(0)
#else
  #define LS_LOG_ID_INVOKE(_level, _mod, _fmt, _param_num, _params, _id) \
    ls_print(__FUNCTION__, sizeof(__FUNCTION__)-1, __LINE__, LS_LOG_LEVEL_##_level, LS_LOG_MOD_##_mod, _fmt LS_LOG_APPEND_ID_PARAMS_INVOKE(_param_num, _params))
#endif /* #if defined(LS_LOG_DYNMICAL_FILTER_SUPPORTED) && (LS_LOG_DYNMICAL_FILTER_SUPPORTED) */
#endif /* #if defined(LS_LOG_ID_SUPPORTED) && (LS_LOG_ID_SUPPORTED) */

#define LS_LOG_ID_PARAM(_no) p##_no
#define LS_LOG_ID_PARAMS_INVOKE(_param_num, _params) LS_LOG_ID_PARAMS_INVOKE_##_param_num _params
#define LS_LOG_APPEND_ID_PARAMS_INVOKE(_param_num, _params) LS_LOG_APPEND_COMMA_##_param_num() LS_LOG_ID_PARAMS_INVOKE_##_param_num _params

#ifndef DOXYGEN_SHOULD_SKIP_THIS
#define LS_LOG_APPEND_COMMA_0()
#define LS_LOG_APPEND_COMMA_1() ,
#define LS_LOG_APPEND_COMMA_2() ,
#define LS_LOG_APPEND_COMMA_3() ,
#define LS_LOG_APPEND_COMMA_4() ,
#define LS_LOG_APPEND_COMMA_5() ,
#define LS_LOG_APPEND_COMMA_6() ,
#define LS_LOG_APPEND_COMMA_7() ,
#define LS_LOG_APPEND_COMMA_8() ,
#define LS_LOG_APPEND_COMMA_9() ,
#define LS_LOG_APPEND_COMMA_10() ,
#define LS_LOG_APPEND_COMMA_11() ,
#define LS_LOG_APPEND_COMMA_12() ,
#define LS_LOG_APPEND_COMMA_13() ,
#define LS_LOG_APPEND_COMMA_14() ,
#define LS_LOG_APPEND_COMMA_15() ,
#define LS_LOG_APPEND_COMMA_16() ,
#define LS_LOG_APPEND_COMMA_17() ,
#define LS_LOG_APPEND_COMMA_18() ,
#define LS_LOG_APPEND_COMMA_19() ,
#define LS_LOG_APPEND_COMMA_20() ,
#define LS_LOG_APPEND_COMMA_21() ,
#define LS_LOG_APPEND_COMMA_22() ,
#define LS_LOG_APPEND_COMMA_23() ,
#define LS_LOG_APPEND_COMMA_24() ,
#define LS_LOG_APPEND_COMMA_25() ,
#define LS_LOG_APPEND_COMMA_26() ,
#define LS_LOG_APPEND_COMMA_27() ,
#define LS_LOG_APPEND_COMMA_28() ,
#define LS_LOG_APPEND_COMMA_29() ,
#define LS_LOG_APPEND_COMMA_30() ,
#define LS_LOG_APPEND_COMMA_31() ,
#define LS_LOG_ID_PARAMS_INVOKE_0()
#define LS_LOG_ID_PARAMS_INVOKE_1(p1) p1
#define LS_LOG_ID_PARAMS_INVOKE_2(p1,p2) p1,p2
#define LS_LOG_ID_PARAMS_INVOKE_3(p1,p2,p3) p1,p2,p3
#define LS_LOG_ID_PARAMS_INVOKE_4(p1,p2,p3,p4) p1,p2,p3,p4
#define LS_LOG_ID_PARAMS_INVOKE_5(p1,p2,p3,p4,p5) p1,p2,p3,p4,p5
#define LS_LOG_ID_PARAMS_INVOKE_6(p1,p2,p3,p4,p5,p6) p1,p2,p3,p4,p5,p6
#define LS_LOG_ID_PARAMS_INVOKE_7(p1,p2,p3,p4,p5,p6,p7) p1,p2,p3,p4,p5,p6,p7
#define LS_LOG_ID_PARAMS_INVOKE_8(p1,p2,p3,p4,p5,p6,p7,p8) p1,p2,p3,p4,p5,p6,p7,p8
#define LS_LOG_ID_PARAMS_INVOKE_9(p1,p2,p3,p4,p5,p6,p7,p8,p9) p1,p2,p3,p4,p5,p6,p7,p8,p9
#define LS_LOG_ID_PARAMS_INVOKE_10(p1,p2,p3,p4,p5,p6,p7,p8,p9,p10) p1,p2,p3,p4,p5,p6,p7,p8,p9,p10
#define LS_LOG_ID_PARAMS_INVOKE_11(p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11) p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11
#define LS_LOG_ID_PARAMS_INVOKE_12(p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12) p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12
#define LS_LOG_ID_PARAMS_INVOKE_13(p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13) p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13
#define LS_LOG_ID_PARAMS_INVOKE_14(p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14) p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14
#define LS_LOG_ID_PARAMS_INVOKE_15(p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15) p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15
#define LS_LOG_ID_PARAMS_INVOKE_16(p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16) p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16
#define LS_LOG_ID_PARAMS_INVOKE_17(p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17) p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17
#define LS_LOG_ID_PARAMS_INVOKE_18(p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18) p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18
#define LS_LOG_ID_PARAMS_INVOKE_19(p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19) p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19
#define LS_LOG_ID_PARAMS_INVOKE_20(p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20) p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20
#define LS_LOG_ID_PARAMS_INVOKE_21(p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21) p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21
#define LS_LOG_ID_PARAMS_INVOKE_22(p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22) p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22
#define LS_LOG_ID_PARAMS_INVOKE_23(p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23) p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23
#define LS_LOG_ID_PARAMS_INVOKE_24(p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24) p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24
#define LS_LOG_ID_PARAMS_INVOKE_25(p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25) p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25
#define LS_LOG_ID_PARAMS_INVOKE_26(p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26) p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26
#define LS_LOG_ID_PARAMS_INVOKE_27(p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27) p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27
#define LS_LOG_ID_PARAMS_INVOKE_28(p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28) p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28
#define LS_LOG_ID_PARAMS_INVOKE_29(p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29) p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29
#define LS_LOG_ID_PARAMS_INVOKE_30(p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30) p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30
#define LS_LOG_ID_PARAMS_INVOKE_31(p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31) p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31
#endif /* #ifndef DOXYGEN_SHOULD_SKIP_THIS */

/*========================================================================================================*/
/* Functions declaration                                                                                  */
/*========================================================================================================*/

extern void ls_print(const char *func, int func_len, int line, int level, int mod, const char *fmt, ...); /* Printing with any arguments, into buffer */
extern void ls_log_id(unsigned int id, int param_num, ...); /* Logging a trace id with interger arguments */

#ifdef __cplusplus
}
#endif

#endif /* __XCC_LS_LOG_H__ */


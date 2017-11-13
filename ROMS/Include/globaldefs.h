/*
** Include file "globaldef.h"
**
** svn $Id$
********************************************************** Hernan G. Arango ***
** Copyright (c) 2002-2017 The ROMS/TOMS Group     Alexander F. Shchepetkin  **
**   Licensed under a MIT/X style license                                    **
**   See License_ROMS.txt                                                    **
*******************************************************************************
**                                                                           **
** WARNING: This  file  contains  a set of  predetermined macro definitions  **
** =======  which are inserted into the individual files by C-preprocessor.  **
** It is strongly recommended to NOT modify any of the definitions below.    **
**                                                                           **
*******************************************************************************
*/

/*
** Set assumed-shape array switch.  Imported arrays with dummy
** arguments that takes the shape of the actual argument passed
** to it.  If off, all the arrays are explicit-shape.  In some
** computer explicit-shape arrays slow down performacnce because
** the arrays are copied when passed by arguments.
*/

#if !((defined G95 && defined I686) || defined UNICOS_SN)
# define ASSUMED_SHAPE
#endif

/*
** Set switch for computer lacking 4-byte (32 bit) floating point
** representation, like some Crays.  This becomes important when
** defining attributes for 4-byte float variables in NetCDF files.
** We need to have the _FillValue attribute of the same type as
** as the NetCDF variable.
*/

#if defined UNICOS_SN
# define NO_4BYTE_REALS
#endif

/*
** Set internal distributed-memory switch.
*/

#if defined MPI
# define DISTRIBUTE
#endif

/*
** Make sure that either "mpi_allgather" or "mpi_allreduce" is used
** in mp_reduce.  Low-level routines give an error.
*/

#ifdef DISTRIBUTE
# if !(defined REDUCE_ALLGATHER || defined REDUCE_ALLREDUCE)
#  define REDUCE_ALLGATHER
# endif
#endif

/*
** Turn ON/OFF time profiling.
*/

#define PROFILE

/*
** Set default time-averaging filter for barotropic fields.
**
*/

#ifdef SOLVE3D
# undef COSINE2
# define POWER_LAW
#endif

/*
** Turn ON/OFF switch to include/disregard the difference between
** rho0 and surface density in the computation of baroclinic pressure
** term.
*/

#define RHO_SURF

/*
** Turn ON/OFF double precision for real type variables and
** associated intrinsic functions.
*/

#define DOUBLE_PRECISION

/*
** Turn ON masking when wetting and drying is activated.
*/

#if !defined MASKING && defined WET_DRY
# define MASKING
#endif

/*
** If wetting and drying, activate limiting of bottom stress.
*/

#if !defined LIMIT_BSTRESS && defined WET_DRY
# define LIMIT_BSTRESS
#endif

/*
** Define macro for the first 2D time-step.
*/

#ifdef SOLVE3D
# define FIRST_2D_STEP iif(ng).eq.1
#else
# define FIRST_2D_STEP iic(ng).eq.ntfirst(ng)
#endif

/*
** Set switch for distributed-memory applications to gather and scatter
** I/O data in 2D slabs. This is necessary on some platforms to conserve
** memory.
*/

#if defined DISTRIBUTE
# if defined UNICOS_SN
#  define INLINE_2DIO
# endif
#endif

/*
** Remove OpenMP directives in serial and distributed memory
** Applications.  This definition will be used in conjunction with
** the pearl script "cpp_clean" to remove the full directive.
*/

#if !defined _OPENMP
# define $OMP !
#endif

/*
** Set tile range for distributed- or shared-memory configurations.
*/

#ifdef DISTRIBUTE
# define THREAD_RANGE MyRank,MyRank
#else
# define THREAD_RANGE 0,numthreads-1
#endif

/*
** Choice of double/single precision for real type variables and
** associated intrinsic functions.
*/

#if (defined CRAY || defined CRAYT3E) && !defined CRAYX1
# ifdef  DOUBLE_PRECISION
#  undef  DOUBLE_PRECISION
# endif
#endif

#ifdef DOUBLE_PRECISION
# ifdef DISTRIBUTE
#  define PDNAUPD pdnaupd
#  define PDNEUPD pdneupd
#  define PDSAUPD pdsaupd
#  define PDSEUPD pdseupd
#  define PDNORM2 pdnorm2
# else
#  define DNAUPD dnaupd
#  define DNEUPD dneupd
#  define DSAUPD dsaupd
#  define DSEUPD dseupd
#  define DNRM2 dnrm2
# endif
# define DAXPY daxpy
# define DSTEQR dsteqr
#else
# ifdef DISTRIBUTE
#  define PDNAUPD psnaupd
#  define PDNEUPD psneupd
#  define PDSAUPD pssaupd
#  define PDSEUPD psseupd
#  define PDNORM2 psnorm2
# else
#  define DNAUPD snaupd
#  define DNEUPD sneupd
#  define DSAUPD ssaupd
#  define DSEUPD sseupd
#  define DNRM2 snrm2
# endif
#  define DAXPY saxpy
#  define DSTEQR ssteqr
#endif

/*
** Set 4DVAR sensitivity switch.
*/

#if defined W4DPSAS_SENSITIVITY || \
    defined W4DVAR_SENSITIVITY
# define SENSITIVITY_4DVAR
#endif

/*
** Set perturbation tangent linear, fine amplitude tangent linear,
** and adjoint model switches.
*/

#if defined ARRAY_MODES         || defined CLIPPING            || \
    defined CORRELATION         || defined FT_EIGENMODES       || \
    defined FORCING_SV          || defined HESSIAN_FSV         || \
    defined HESSIAN_SO          || defined HESSIAN_SV          || \
    defined INNER_PRODUCT       || defined IS4DVAR             || \
    defined IS4DVAR_SENSITIVITY || defined OPT_PERTURBATION    || \
    defined OPT_OBSERVATIONS    || defined PICARD_TEST         || \
    defined R_SYMMETRY          || defined RPM_DRIVER          || \
    defined SANITY_CHECK        || defined SENSITIVITY_4DVAR   || \
    defined STOCHASTIC_OPT      || defined TLM_CHECK           || \
    defined TLM_DRIVER          || defined TL_W4DPSAS          || \
    defined TL_W4DVAR           || defined W4DPSAS             || \
    defined W4DVAR
# define TANGENT
#endif
#if defined AD_SENSITIVITY      || defined ADM_DRIVER          || \
    defined AFT_EIGENMODES      || defined ARRAY_MODES         || \
    defined CLIPPING            || defined CORRELATION         || \
    defined FORCING_SV          || defined HESSIAN_SO          || \
    defined HESSIAN_FSV         || defined HESSIAN_SV          || \
    defined INNER_PRODUCT       || defined IS4DVAR             || \
    defined IS4DVAR_SENSITIVITY || defined OPT_PERTURBATION    || \
    defined OPT_OBSERVATIONS    || defined R_SYMMETRY          || \
    defined SANITY_CHECK        || defined SENSITIVITY_4DVAR   || \
    defined SO_SEMI             || defined STOCHASTIC_OPT      || \
    defined TLM_CHECK           || defined TL_W4DPSAS          || \
    defined TL_W4DVAR           || defined W4DPSAS             || \
    defined W4DVAR
# define ADJOINT
#endif
#if defined PICARD_TEST        || defined RPM_DRIVER         || \
    defined TL_W4DVAR          || defined W4DVAR             || \
    defined W4DVAR_SENSITIVITY || defined ARRAY_MODES        || \
    defined CLIPPING
# define TL_IOMS
#endif
#if !defined ANA_PERTURB                                 && \
    (defined CORRELATION     || defined SANITY_CHECK     || \
     defined R_SYMMETRY)
# define ANA_PERTURB
#endif

/*
** Since some of the tracer advection alogorithms are highly nonlinear,
** it is possible to choose a simpler (less nonlinear) horizontal and
** vertical tracer advection option for the tangent linear, representer
** and adjoint routines. This is likely to improve the convergence of
** the 4DVar algorithms. Notice that this strategy still allows us to
** use highly nonlinear tracer advection schemes in the basic state
** when running the nonlinear model.
*/

#if defined TANGENT || defined TL_IOMS || defined ADJOINT
# if !defined TS_A4HADVECTION_TL       && \
     !defined TS_C2HADVECTION_TL       && \
     !defined TS_C4HADVECTION_TL       && \
     !defined TS_U3HADVECTION_TL
#  if defined TS_A4HADVECTION
#   define TS_A4HADVECTION_TL
#  elif defined TS_C2HADVECTION
#   define TS_C2HADVECTION_TL
#  elif defined TS_C4HADVECTION
#   define TS_C4HADVECTION_TL
#  elif defined TS_U3HADVECTION
#   define TS_U3HADVECTION_TL
#  endif
# endif

# if !defined TS_A4VADVECTION_TL       && \
     !defined TS_C2VADVECTION_TL       && \
     !defined TS_C4VADVECTION_TL       && \
     !defined TS_SVADVECTION_TL
#  if defined TS_A4VADVECTION
#   define TS_A4VADVECTION_TL
#  elif defined TS_C2VADVECTION
#   define TS_C2VADVECTION_TL
#  elif defined TS_C4VADVECTION
#   define TS_C4VADVECTION_TL
#  elif defined TS_SVADVECTION
#   define TS_SVADVECTION_TL
#  endif
# endif
#endif

/*
** Now, we need a switch the tracer advection schemes that
** have not adjointed yet.
*/

#if defined TANGENT || defined TL_IOMS || defined ADJOINT
# if defined TS_A4HADVECTION_TL || defined TS_C2HADVECTION_TL || \
     defined TS_C4HADVECTION_TL || defined TS_U3HADVECTION_TL
#  define TS_HADVECTION_TL
# endif

# if defined TS_A4VADVECTION_TL || defined TS_C2VADVECTION_TL || \
     defined TS_C4VADVECTION_TL || defined TS_SVADVECTION_TL
#  define TS_VADVECTION_TL
# endif
#endif

/*
** Turn off nonlinear model switch.
*/

#define NONLINEAR
#if defined AD_SENSITIVITY   || defined ADM_DRIVER       || \
    defined AFT_EIGENMODES   || defined FORCING_SV       || \
    defined FT_EIGENMODES    || defined HESSIAN_FSV      || \
    defined HESSIAN_SO       || defined HESSIAN_SV       || \
    defined INNER_PRODUCT    || defined OPT_OBSERVATIONS || \
    defined OPT_PERTURBATION || defined PICARD_TEST      || \
    defined RPM_DRIVER       || defined SANITY_CHECK     || \
    defined SO_SEMI          || defined STOCHASTIC_OPT   || \
    defined TLM_DRIVER
# undef NONLINEAR
#endif

/*
** Activate bacroclinic pressure gradient response due to the
** perturbation of free-surface in the presence of stratification
** and bathymetry. This option does not pass the sanity check
** in adjoint and tangent linear applications.
*/

#ifdef SOLVE3D
# if !(defined ADJOINT || defined TANGENT)
#   define VAR_RHO_2D
# endif
#endif

/*
** Set output index for multi-time levels variables.
*/

#ifdef SOLVE3D
# if defined TANGENT || defined TL_IOMS
#  define TKOUT kstp(ng)
#  define KOUT kstp(ng)
#  define NOUT nrhs(ng)
# else
#  define KOUT kstp(ng)
#  define NOUT nrhs(ng)
# endif
#else
# if defined TANGENT || defined TL_IOMS
#  define TKOUT kstp(ng)
# endif
# define KOUT knew(ng)
#endif

/*
** Set internal switch for the need of a propagator driver.
*/

#if defined AFT_EIGENMODES   || defined ENSEMBLE         || \
    defined FORCING_SV       || defined FT_EIGENMODES    || \
    defined HESSIAN_FSV      || defined HESSIAN_SO       || \
    defined HESSIAN_SV       || defined OPT_PERTURBATION || \
    defined PSEUDOSPECTRA    || defined SO_SEMI          || \
    defined SO_TRACE         || defined STOCHASTIC_OPT
# define PROPAGATOR
#endif

/*
** Activate processing of forward vertical mixing.
**
*/

#if !defined FORWARD_MIXING  && \
    (defined TANGENT         || defined TL_IOMS    || \
     defined ADJOINT)        && \
    (defined LMD_MIXING      || defined GLS_MIXING || \
     defined MY25_MIXING)
# define FORWARD_MIXING
#endif

/*
** Set internal switches for all the 4DVAR schemes.
*/

#if !defined WEAK_CONSTRAINT     && \
    (defined ARRAY_MODES         || defined CLIPPING            || \
     defined R_SYMMETRY          || defined TL_W4DPSAS          || \
     defined TL_W4DVAR           || defined W4DPSAS             || \
     defined W4DVAR              || defined W4DPSAS_SENSITIVITY || \
     defined W4DVAR_SENSITIVITY)
# define WEAK_CONSTRAINT
#endif
#if !defined WEAK_CONSTRAINT     && defined RPM_RELAXATION
# undef RPM_RELAXATION
#endif
#if defined CORRELATION          || defined HESSIAN_FSV         || \
    defined HESSIAN_SO           || defined HESSIAN_SV          || \
    defined IS4DVAR              || defined IS4DVAR_SENSITIVITY || \
    defined OPT_OBSERVATIONS     || defined TLM_CHECK           || \
    defined WEAK_CONSTRAINT
# define FOUR_DVAR
#endif
#if defined IS4DVAR
# define BACKGROUND
#endif
#if !(defined W4DPSAS || defined W4DVAR) && defined POSTERIOR_EOFS
# undef POSTERIOR_EOFS
#endif
#if !(defined W4DPSAS || defined W4DVAR) && defined POSTERIOR_ERROR_F
# undef POSTERIOR_ERROR_F
#endif
#if !(defined W4DPSAS || defined W4DVAR) && defined POSTERIOR_ERROR_I
# undef POSTERIOR_ERROR_I
#endif
#if !(defined WEAK_CONSTRAINT || defined IS4DVAR_SENSITIVITY) && \
      defined OBS_IMPACT
# undef OBS_IMPACT
#endif

/*
** Activate internal switch to process 4DVAR observations.
*/

#if defined IS4DVAR            || defined IS4DVAR_SENSITIVITY || \
    defined SENSITIVITY_4DVAR  || defined TLM_CHECK           || \
    defined TL_W4DPSAS         || defined TL_W4DVAR           || \
    defined VERIFICATION       || defined W4DPSAS             || \
    defined W4DVAR             || defined ARRAY_MODES         || \
    defined CLIPPING
# define OBSERVATIONS
#endif

#if defined IS4DVAR            || defined IS4DVAR_SENSITIVITY || \
    defined R_SYMMETRY         || defined SENSITIVITY_4DVAR   || \
    defined TLM_CHECK          || defined TL_W4DPSAS          || \
    defined TL_W4DVAR          || defined W4DPSAS             || \
    defined W4DVAR             || defined ARRAY_MODES         || \
    defined CLIPPING
# define TLM_OBS
#endif

/*
** Activate reading and writting of the basic sate.
*/

#if !defined FORWARD_READ      && \
    (defined ARRAY_MODES       || defined CLIPPING            || \
     defined IS4DVAR           || defined IS4DVAR_SENSITIVITY || \
     defined PROPAGATOR        || defined SENSITIVITY_4DVAR   || \
     defined TL_W4DPSAS        || defined TL_W4DVAR           || \
     defined W4DPSAS           || defined W4DVAR)
# define FORWARD_READ
#endif
#if !defined FORWARD_WRITE     && \
    (defined ARRAY_MODES       || defined CLIPPING            || \
     defined IS4DVAR           || defined IS4DVAR_SENSITIVITY || \
     defined SENSITIVITY_4DVAR || defined TL_W4DPSAS          || \
     defined TL_W4DVAR         || defined W4DPSAS             || \
     defined W4DVAR)
# define FORWARD_WRITE
#endif

/*
** Set internal weak constraint switches.
*/

#ifdef WEAK_CONSTRAINT
# define IMPULSE
#endif

/*
** Set in internal switch to activate computation of nonlinear
** equation of state expnasion polynomial T-derivatives.
*/

#if defined LMD_SKPP || defined LMD_BKPP || defined BULK_FLUXES || \
    defined TANGENT  || defined TL_IOMS  || defined ADJOINT
# define EOS_TDERIVATIVE
#endif

/*
** If splines, deactivate horizontal and vertical smoothing of
** Richardson number horizontally and/or vertically.
*/

#ifdef RI_SPLINES
# if defined LMD_MIXING
#  undef RI_HORAVG
#  undef RI_VERAVG
# endif
#endif

/*
** Activate internal switch for the computation of the Brunt-Vaisala
** frequency.
*/

#if defined BVF_MIXING || defined LMD_MIXING  || defined LMD_SKPP    || \
    defined LMD_BKPP   || defined GLS_MIXING  || defined MY25_MIXING
# define BV_FREQUENCY
#endif

/*
** Activate internal switch for bottom boundary layer closure.
*/

#if defined SSW_BBL || defined MB_BBL || defined SG_BBL
# define BBL_MODEL
#endif

/*
** Check if spatially varying bottom friction parameters are needed.
*/

#if defined UV_DRAG_GRID && \
  !(defined BBL_MODEL    || defined SEDIMENT || \
    defined UV_LOGDRAG   || defined UV_LDRAG || \
    defined UV_QDRAG)
# undef UV_DRAG_GRID
#endif
#if defined ANA_DRAG     && !defined UV_DRAG_GRID
# undef ANA_DRAG
#endif

/*
** Activate internal switch for imposing REFDIF as a
** monochromatic wave driver.
*/

#if defined REFDIF_COUPLING && defined SVENDSEN_ROLLER
# define MONO_ROLLER
#endif

/*
** Activate internal biology option when using any type of biological
** module.
*/

#if defined BIO_FENNEL  || defined ECOSIM      || \
    defined HYPOXIA_SRM || defined NEMURO      || \
    defined NPZD_FRANKS || defined NPZD_IRON   || \
    defined NPZD_POWELL || defined RED_TIDE
# define BIOLOGY
#endif

/*
** Activate internal option for biological float behavior.
*/

#if defined FLOATS && defined FLOAT_OYSTER
# define FLOAT_BIOLOGY
#endif

/*
** Define internal option to couple to other models.
**
*/

#if defined COAMPS_COUPLING || defined REGCM_COUPLING || \
    defined WRF_COUPLING
# define ATM_COUPLING
#endif

#if defined CICE_COUPLING
  define ICE_COUPLING
#endif

#if defined REFDIF_COUPLING || defined SWAN_COUPLING || \
    defined WAM_COUPLING
# define WAV_COUPLING
#endif

#if defined ATM_COUPLING || ICE_COUPLING || WAV_COUPLING
# define MODEL_COUPLING
#endif

/*
** Define internal option for radiation stress forcing.
*/

#if defined NEARSHORE_MELLOR05 || defined NEARSHORE_MELLOR08
# define NEARSHORE_MELLOR
#endif
#if defined NEARSHORE_MELLOR
# define NEARSHORE
#endif

/*
** Define internal option to process wave data.
*/

#if defined BBL_MODEL    || defined NEARSHORE || \
    defined WAV_COUPLING
# define WAVES_DIR
#endif

#if  defined BBL_MODEL   && \
   !(defined SSW_CALC_UB || defined MB_CALC_UB ||  \
     defined SG_CALC_UB)
# define WAVES_UB
#endif

#if (defined BBL_MODEL        && !defined WAVES_UB)           || \
     defined NEARSHORE        || \
     defined ZOS_HSIG         || defined COARE_TAYLOR_YELLAND || \
     defined BEDLOAD_SOULSBY  || defined WAV_COUPLING
# define WAVES_HEIGHT
#endif

#if defined NEARSHORE    || defined BEDLOAD_SOULSBY || \
    defined WAV_COUPLING
# define WAVES_LENGTH
#endif

#if (!defined DEEPWATER_WAVES      && \
     (defined COARE_TAYLOR_YELLAND || defined COARE_OOST)) || \
      defined NEARSHORE_MELLOR     || \
      defined BEDLOAD_SOULSBY      || defined WAV_COUPLING
# define WAVES_LENGTH
#endif

#if defined COARE_TAYLOR_YELLAND   || defined COARE_OOST || \
    defined WAV_COUPLING
# define WAVES_TOP_PERIOD
#endif

#if defined BBL_MODEL || defined WAV_COUPLING
# define WAVES_BOT_PERIOD
#endif

#if !defined WAV_COUPLING    && \
   ((defined BULK_FLUXES     && defined COARE_TAYLOR_YELLAND) || \
    (defined BULK_FLUXES     && defined COARE_OOST)           || \
     defined SVENDSEN_ROLLER || defined TKE_WAVEDISS          || \
     defined WAVES_DIR       || defined WAVES_BOT_PERIOD      || \
     defined WAVES_HEIGHT    || defined WAVES_TOP_PERIOD      || \
     defined WAVES_LENGTH)
# define WAVE_DATA
#endif

/*
** Define internal option for bedload treatment.
*/

#if defined BEDLOAD_MPM || defined BEDLOAD_SOULSBY
# define BEDLOAD
#endif

/*
** Define internal flag indicating processing of input forcing
** NetCDF file.
*/

#ifdef SOLVE3D
# ifdef BULK_FLUXES
#  ifdef ANA_SMFLUX
#   undef ANA_SMFLUX
#  endif
#  ifdef ANA_STFLUX
#   undef ANA_STFLUX
#  endif
# endif
# if !defined ANA_BTFLUX   || \
    (!defined ATM_COUPLING && \
     !defined BULK_FLUXES  && !defined ANA_SMFLUX)   || \
    (!defined ATM_COUPLING && \
     !defined BULK_FLUXES  && !defined ANA_STFLUX)   || \
    ( defined BIOLOGY      && !defined ANA_SPFLUX)   || \
    ( defined BIOLOGY      && !defined ANA_BPFLUX)   || \
    (!defined ATM_COUPLING && \
      defined BULK_FLUXES  && !defined LONGWAVE)     || \
    (!defined ATM_COUPLING && \
      defined BULK_FLUXES  && !defined ANA_PAIR)     || \
    (!defined ATM_COUPLING && \
      defined BULK_FLUXES  && !defined ANA_TAIR)     || \
    (!defined ATM_COUPLING && \
      defined BULK_FLUXES  && !defined ANA_HUMIDITY) || \
    (!defined ATM_COUPLING && \
      defined BULK_FLUXES  && !defined ANA_CLOUD)    || \
    (!defined ATM_COUPLING && \
      defined BULK_FLUXES  && !defined ANA_RAIN)     || \
    (!defined ATM_COUPLING && \
      defined BULK_FLUXES  && !defined ANA_WINDS)    || \
    (!defined ATM_COUPLING && \
      defined BULK_FLUXES  && !defined ANA_SRFLUX)   || \
    (!defined ATM_COUPLING && \
      defined LMD_SKPP     && !defined ANA_SRFLUX)   || \
    (!defined ATM_COUPLING && \
      defined RED_TIDE)    || \
    (!defined ATM_COUPLING && \
      defined SALINITY     && !defined ANA_SSFLUX    && \
     (defined BULK_FLUXES  && !defined EMINUSP))     || \
    (!defined ATM_COUPLING && \
      defined SOLAR_SOURCE && !defined ANA_SRFLUX)   || \
    ( defined BBL_MODEL    && (!defined ANA_WWAVE    && \
     !defined WAV_COUPLING))                         || \
    ( defined SEDIMENT     && !defined ANA_SPFLUX)   || \
    ( defined SEDIMENT     && !defined ANA_BPFLUX)   || \
    ( defined WAVE_DATA    && (!defined ANA_WWAVE    && \
     !defined WAV_COUPLING))
#  define FRC_FILE
# endif
#else
# if(!defined ATM_COUPLING    && !defined ANA_SMFLUX)
#  define FRC_FILE
# endif
#endif

/*
** Check analytical initial conditions options.
*/

#if defined ANA_BIOLOGY && !defined BIOLOGY
# undef ANA_BIOLOGY
#endif
#if defined ANA_PASSIVE && !defined T_PASSIVE
# undef ANA_PASSIVE
#endif
#if defined ANA_SEDIMENT && !(defined SEDIMENT || defined BBL_MODEL)
# undef ANA_SEDIMENT
#endif
#if  !defined ANA_INITIAL || \
    ( defined BIOLOGY     && !defined ANA_BIOLOGY)  || \
    ( defined T_PASSIVE   && !defined ANA_PASSIVE)  || \
    ( defined SEDIMENT    && !defined ANA_SEDIMENT) || \
    ( defined BBL_MODEL   && !defined ANA_SEDIMENT)
# define INI_FILE
#endif

/*
** Define internal shortwave radiation option.  Undefine analytical
** shortwave option if not needed.
*/

#if defined LMD_SKPP     || defined SOLAR_SOURCE   || \
    defined BULK_FLUXES  || defined BIOLOGY
# define SHORTWAVE
#endif
#if !defined SHORTWAVE   && defined ANA_SRFLUX
# undef ANA_SRFLUX
#endif
#if !defined SHORTWAVE   && defined DIURNAL_SRFLUX
# undef DIURNAL_SRFLUX
#endif

/*
** Define internal clouds option.  Undefine analytical
** shortwave option if not needed.
*/

#if (defined BULK_FLUXES && defined LONGWAVE) || defined ECOSIM || \
    (defined ANA_SRFLUX  && defined ALBEDO)
# define CLOUDS
#endif
#if !defined CLOUDS && defined ANA_CLOUD
# undef ANA_CLOUD
#endif

/*
** Check if it is meaningful to write out momentum/tracer diagnostics
** and activate internal diagnostics option.
*/

#if !defined SOLVE3D || defined TS_FIXED
# if defined DIAGNOSTICS_TS
#   undef DIAGNOSTICS_TS
# endif
#endif
#if defined DIAGNOSTICS_BIO && \
  !(defined BIO_FENNEL      || defined HYPOXIA_SRM)
#  undef DIAGNOSTICS_BIO
#endif
#if defined DIAGNOSTICS_BIO || defined DIAGNOSTICS_TS || \
    defined DIAGNOSTICS_UV
# define DIAGNOSTICS
#endif

/*
** Check if any analytical expression is defined.
*/

#if defined ANA_BIOLOGY    || defined ANA_BPFLUX      || \
    defined ANA_BSFLUX     || defined ANA_BTFLUX      || \
    defined ANA_CLOUD      || defined ANA_DIAG        || \
    defined ANA_DQDSST     || defined ANA_DRAG        || \
    defined ANA_FSOBC      || defined ANA_GRID        || \
    defined ANA_HUMIDITY   || defined ANA_INITIAL     || \
    defined ANA_M2CLIMA    || defined ANA_M2OBC       || \
    defined ANA_M3CLIMA    || defined ANA_M3OBC       || \
    defined ANA_MASK       || defined ANA_NUDGCOEF    || \
    defined ANA_PAIR       || defined ANA_PASSIVE     || \
    defined ANA_PERTURB    || defined ANA_PSOURCE     || \
    defined ANA_RAIN       || defined ANA_RESPIRATION || \
    defined ANA_SEDIMENT   || defined ANA_SMFLUX      || \
    defined ANA_SPFLUX     || defined ANA_SPINNING    || \
    defined ANA_SPONGE     || defined ANA_SRFLUX      || \
    defined ANA_SSFLUX     || defined ANA_SSH         || \
    defined ANA_SSS        || defined ANA_SST         || \
    defined ANA_STFLUX     || defined ANA_TAIR        || \
    defined ANA_TCLIMA     || defined ANA_TOBC        || \
    defined ANA_VMIX       || defined ANA_WINDS       || \
    defined ANA_WWAVE      || defined DIFF_GRID       || \
    defined VISC_GRID
# define ANALYTICAL
#endif

/*
** If splitting 3rd-order upstream bias horizontal advection of
** tracer, activate other needed flags.
*/

#ifdef TS_U3ADV_SPLIT
# define DIFF_3DCOEF
# ifdef TS_U3HADVECTION
#  undef TS_U3HADVECTION
# endif
# ifndef TS_C4HADVECTION
#  define TS_C4HADVECTION
# endif
# ifndef TS_C4VADVECTION
#  define TS_C4VADVECTION
# endif
# ifndef TS_DIF4
#  define TS_DIF4
# endif
# ifdef TS_DIF2
#  undef TS_DIF2
# endif
# ifdef TS_SMAGORINSKY
#  undef TS_SMAGORINSKY
# endif
#endif

/*
** If splitting 3rd-order upstream bias horizontal advection of
** momentum, activate other needed flags.
*/

#ifdef UV_U3ADV_SPLIT
# define VISC_3DCOEF
# ifndef UV_C4ADVECTION
#  define UV_C4ADVECTION
# endif
# ifndef UV_VIS4
#  define UV_VIS4
# endif
# ifdef UV_VIS2
#  undef UV_VIS2
# endif
# ifdef UV_SMAGORINSKY
#  undef UV_SMAGORINSKY
# endif
#endif

/*
** Define internal switch for Smagorinsky-like mixing.
*/

#if !defined DIFF_3DCOEF && defined TS_SMAGORINSKY
# define DIFF_3DCOEF
#endif
#if !defined VISC_3DCOEF && defined UV_SMAGORINSKY
# define VISC_3DCOEF
#endif

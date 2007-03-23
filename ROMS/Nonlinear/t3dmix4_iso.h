#ifdef EW_PERIODIC
# define I_RANGE Istr-1,Iend+1
#else
# define I_RANGE MAX(Istr-1,1),MIN(Iend+1,Lm(ng))
#endif
#ifdef NS_PERIODIC
# define J_RANGE Jstr-1,Jend+1
#else
# define J_RANGE MAX(Jstr-1,1),MIN(Jend+1,Mm(ng))
#endif

      SUBROUTINE t3dmix4 (ng, tile)
!
!svn $Id$
!***********************************************************************
!  Copyright (c) 2002-2007 The ROMS/TOMS Group                         !
!    Licensed under a MIT/X style license                              !
!    See License_ROMS.txt                           Hernan G. Arango   !
!****************************************** Alexander F. Shchepetkin ***
!                                                                      !
!  This subroutine computes horizontal biharmonic mixing of tracers    !
!  along isopycnic surfaces.                                           !
!                                                                      !
!***********************************************************************
!
      USE mod_param
#ifdef DIAGNOSTICS_TS
      USE mod_diags
#endif
      USE mod_grid
      USE mod_mixing
      USE mod_ocean
      USE mod_stepping
!
!  Imported variable declarations.
!
      integer, intent(in) :: ng, tile
!
!  Local variable declarations.
!
#include "tile.h"
!
#ifdef PROFILE
      CALL wclock_on (ng, iNLM, 29)
#endif
      CALL t3dmix4_tile (ng, Istr, Iend, Jstr, Jend,                    &
     &                   LBi, UBi, LBj, UBj,                            &
     &                   nrhs(ng), nnew(ng),                            &
#ifdef MASKING
     &                   GRID(ng) % umask,                              &
     &                   GRID(ng) % vmask,                              &
#endif
     &                   GRID(ng) % om_v,                               &
     &                   GRID(ng) % on_u,                               &
     &                   GRID(ng) % pm,                                 &
     &                   GRID(ng) % pn,                                 &
     &                   GRID(ng) % Hz,                                 &
     &                   GRID(ng) % z_r,                                &
     &                   MIXING(ng) % diff4,                            &
     &                   OCEAN(ng) % rho,                               &
#ifdef DIAGNOSTICS_TS
     &                   DIAGS(ng) % DiaTwrk,                           &
#endif
     &                   OCEAN(ng) % t)
#ifdef PROFILE
      CALL wclock_off (ng, iNLM, 29)
#endif
      RETURN
      END SUBROUTINE t3dmix4
!
!***********************************************************************
      SUBROUTINE t3dmix4_tile (ng, Istr, Iend, Jstr, Jend,              &
     &                         LBi, UBi, LBj, UBj,                      &
     &                         nrhs, nnew,                              &
#ifdef MASKING
     &                         umask, vmask,                            &
#endif
     &                         om_v, on_u, pm, pn,                      &
     &                         Hz, z_r,                                 &
     &                         diff4,                                   &
     &                         rho,                                     &
#ifdef DIAGNOSTICS_TS
     &                         DiaTwrk,                                 &
#endif
     &                         t)
!***********************************************************************
!
      USE mod_param
      USE mod_scalars
!
!  Imported variable declarations.
!
      integer, intent(in) :: ng, Iend, Istr, Jend, Jstr
      integer, intent(in) :: LBi, UBi, LBj, UBj
      integer, intent(in) :: nrhs, nnew

#ifdef ASSUMED_SHAPE
# ifdef MASKING
      real(r8), intent(in) :: umask(LBi:,LBj:)
      real(r8), intent(in) :: vmask(LBi:,LBj:)
# endif
      real(r8), intent(in) :: diff4(LBi:,LBj:,:)
      real(r8), intent(in) :: om_v(LBi:,LBj:)
      real(r8), intent(in) :: on_u(LBi:,LBj:)
      real(r8), intent(in) :: pm(LBi:,LBj:)
      real(r8), intent(in) :: pn(LBi:,LBj:)
      real(r8), intent(in) :: Hz(LBi:,LBj:,:)
      real(r8), intent(in) :: z_r(LBi:,LBj:,:)
      real(r8), intent(in) :: rho(LBi:,LBj:,:)
# ifdef DIAGNOSTICS_TS
      real(r8), intent(inout) :: DiaTwrk(LBi:,LBj:,:,:,:)
# endif
      real(r8), intent(inout) :: t(LBi:,LBj:,:,:,:)
#else
# ifdef MASKING
      real(r8), intent(in) :: umask(LBi:UBi,LBj:UBj)
      real(r8), intent(in) :: vmask(LBi:UBi,LBj:UBj)
# endif
      real(r8), intent(in) :: diff4(LBi:UBi,LBj:UBj,NT(ng))
      real(r8), intent(in) :: om_v(LBi:UBi,LBj:UBj)
      real(r8), intent(in) :: on_u(LBi:UBi,LBj:UBj)
      real(r8), intent(in) :: pm(LBi:UBi,LBj:UBj)
      real(r8), intent(in) :: pn(LBi:UBi,LBj:UBj)
      real(r8), intent(in) :: Hz(LBi:UBi,LBj:UBj,N(ng))
      real(r8), intent(in) :: z_r(LBi:UBi,LBj:UBj,N(ng))
      real(r8), intent(in) :: rho(LBi:UBi,LBj:UBj,N(ng))
# ifdef DIAGNOSTICS_TS
      real(r8), intent(inout) :: DiaTwrk(LBi:UBi,LBj:UBj,N(ng),NT(ng),  &
     &                                   NDT)
# endif
      real(r8), intent(inout) :: t(LBi:UBi,LBj:UBj,N(ng),3,NT(ng))
#endif
!
!  Local variable declarations.
!
      integer :: IstrR, IendR, JstrR, JendR, IstrU, JstrV
      integer :: i, itrc, j, k, k1, k2

      real(r8), parameter :: eps = 0.5_r8
      real(r8), parameter :: small = 1.0E-14_r8
      real(r8), parameter :: slope_max = 0.0001_r8
      real(r8), parameter :: strat_min = 0.1_r8

      real(r8) :: cff, cff1, cff2, cff3, cff4, fac

      real(r8), dimension(PRIVATE_2D_SCRATCH_ARRAY,N(ng)) :: LapT

      real(r8), dimension(PRIVATE_2D_SCRATCH_ARRAY) :: FE
      real(r8), dimension(PRIVATE_2D_SCRATCH_ARRAY) :: FX

      real(r8), dimension(PRIVATE_2D_SCRATCH_ARRAY,2) :: FS
      real(r8), dimension(PRIVATE_2D_SCRATCH_ARRAY,2) :: dRde
      real(r8), dimension(PRIVATE_2D_SCRATCH_ARRAY,2) :: dRdx
      real(r8), dimension(PRIVATE_2D_SCRATCH_ARRAY,2) :: dTde
      real(r8), dimension(PRIVATE_2D_SCRATCH_ARRAY,2) :: dTdr
      real(r8), dimension(PRIVATE_2D_SCRATCH_ARRAY,2) :: dTdx

#include "set_bounds.h"
!
!-----------------------------------------------------------------------
!  Compute horizontal biharmonic diffusion along isopycnic surfaces.
!  The biharmonic operator is computed by applying the harmonic
!  operator twice.
!-----------------------------------------------------------------------
!
!  Compute horizontal and density gradients.  Notice the recursive
!  blocking sequence.  The vertical placement of the gradients is:
!
!        dTdx,dTde(:,:,k1) k     rho-points
!        dTdx,dTde(:,:,k2) k+1   rho-points
!          FS,dTdr(:,:,k1) k-1/2   W-points
!          FS,dTdr(:,:,k2) k+1/2   W-points
!
      T_LOOP : DO itrc=1,NT(ng)
        k2=1
        K_LOOP1 : DO k=0,N(ng)
          k1=k2
          k2=3-k1
          IF (k.lt.N(ng)) THEN
            DO j=J_RANGE
              DO i=I_RANGE+1
                cff=0.5_r8*(pm(i,j)+pm(i-1,j))
#ifdef MASKING
                cff=cff*umask(i,j)
#endif
                dRdx(i,j,k2)=cff*(rho(i  ,j,k+1)-                       &
     &                            rho(i-1,j,k+1))
                dTdx(i,j,k2)=cff*(t(i  ,j,k+1,nrhs,itrc)-               &
     &                            t(i-1,j,k+1,nrhs,itrc))
              END DO
            END DO
            DO j=J_RANGE+1
              DO i=I_RANGE
                cff=0.5_r8*(pn(i,j)+pn(i,j-1))
#ifdef MASKING
                cff=cff*vmask(i,j)
#endif
                dRde(i,j,k2)=cff*(rho(i,j  ,k+1)-                       &
     &                            rho(i,j-1,k+1))
                dTde(i,j,k2)=cff*(t(i,j  ,k+1,nrhs,itrc)-               &
     &                            t(i,j-1,k+1,nrhs,itrc))
              END DO
            END DO
          END IF
          IF ((k.eq.0).or.(k.eq.N(ng))) THEN
            DO j=-1+J_RANGE+1
              DO i=-1+I_RANGE+1
                dTdr(i,j,k2)=0.0_r8
                FS(i,j,k2)=0.0_r8
              END DO
            END DO
          ELSE
            DO j=-1+J_RANGE+1
              DO i=-1+I_RANGE+1
#if defined MAX_SLOPE
                cff1=SQRT(dRdx(i,j,k2)**2+dRdx(i+1,j,k2)**2+            &
     &                    dRdx(i,j,k1)**2+dRdx(i+1,j,k1)**2+            &
     &                    dRde(i,j,k2)**2+dRde(i,j+1,k2)**2+            &
     &                    dRde(i,j,k1)**2+dRde(i,j+1,k1)**2)
                cff2=0.25_r8*slope_max*                                 &
     &               (z_r(i,j,k+1)-z_r(i,j,k))*cff1
                cff3=MAX(rho(i,j,k)-rho(i,j,k+1),small)
                cff4=MAX(cff2,cff3)
                cff=-1.0_r8/cff4
#elif defined MIN_STRAT
                cff1=MAX(rho(i,j,k)-rho(i,j,k+1),                       &
     &                   strat_min*(z_r(i,j,k+1)-z_r(i,j,k)))
                cff=-1.0_r8/cff1
#else
                cff1=MAX(rho(i,j,k)-rho(i,j,k+1),eps)
                cff=-1.0_r8/cff1
#endif
                dTdr(i,j,k2)=cff*(t(i,j,k+1,nrhs,itrc)-                 &
     &                            t(i,j,k  ,nrhs,itrc))
                FS(i,j,k2)=cff*(z_r(i,j,k+1)-                           &
     &                          z_r(i,j,k  ))
              END DO
            END DO
          END IF
          IF (k.gt.0) THEN
            DO j=J_RANGE
              DO i=I_RANGE+1
                cff=0.25_r8*(diff4(i,j,itrc)+diff4(i-1,j,itrc))*        &
     &              on_u(i,j)
                FX(i,j)=cff*                                            &
     &                  (Hz(i,j,k)+Hz(i-1,j,k))*                        &
     &                  (dTdx(i,j,k1)-                                  &
     &                   0.5_r8*(MAX(dRdx(i,j,k1),0.0_r8)*              &
     &                              (dTdr(i-1,j,k1)+                    &
     &                               dTdr(i  ,j,k2))+                   &
     &                           MIN(dRdx(i,j,k1),0.0_r8)*              &
     &                              (dTdr(i-1,j,k2)+                    &
     &                               dTdr(i  ,j,k1))))
              END DO
            END DO
            DO j=J_RANGE+1
              DO i=I_RANGE
                cff=0.25_r8*(diff4(i,j,itrc)+diff4(i,j-1,itrc))*        &
     &              om_v(i,j)
                FE(i,j)=cff*                                            &
     &                  (Hz(i,j,k)+Hz(i,j-1,k))*                        &
     &                  (dTde(i,j,k1)-                                  &
     &                   0.5_r8*(MAX(dRde(i,j,k1),0.0_r8)*              &
     &                              (dTdr(i,j-1,k1)+                    &
     &                               dTdr(i,j  ,k2))+                   &
     &                           MIN(dRde(i,j,k1),0.0_r8)*              &
     &                              (dTdr(i,j-1,k2)+                    &
     &                               dTdr(i,j  ,k1))))
              END DO
            END DO
            IF (k.lt.N(ng)) THEN
              DO j=J_RANGE
                DO i=I_RANGE
                  fac=0.5_r8*diff4(i,j,itrc)
                  cff1=MAX(dRdx(i  ,j,k1),0.0_r8)
                  cff2=MAX(dRdx(i+1,j,k2),0.0_r8)
                  cff3=MIN(dRdx(i  ,j,k2),0.0_r8)
                  cff4=MIN(dRdx(i+1,j,k1),0.0_r8)
                  cff=cff1*(cff1*dTdr(i,j,k2)-dTdx(i  ,j,k1))+          &
     &                cff2*(cff2*dTdr(i,j,k2)-dTdx(i+1,j,k2))+          &
     &                cff3*(cff3*dTdr(i,j,k2)-dTdx(i  ,j,k2))+          &
     &                cff4*(cff4*dTdr(i,j,k2)-dTdx(i+1,j,k1))
                  cff1=MAX(dRde(i,j  ,k1),0.0_r8)
                  cff2=MAX(dRde(i,j+1,k2),0.0_r8)
                  cff3=MIN(dRde(i,j  ,k2),0.0_r8)
                  cff4=MIN(dRde(i,j+1,k1),0.0_r8)
                  cff=cff+                                              &
     &                cff1*(cff1*dTdr(i,j,k2)-dTde(i,j  ,k1))+          &
     &                cff2*(cff2*dTdr(i,j,k2)-dTde(i,j+1,k2))+          &
     &                cff3*(cff3*dTdr(i,j,k2)-dTde(i,j  ,k2))+          &
     &                cff4*(cff4*dTdr(i,j,k2)-dTde(i,j+1,k1))
                  FS(i,j,k2)=fac*cff*FS(i,j,k2)
                END DO
              END DO
            END IF
!
!  Compute first harmonic operator, without mixing coefficient.
!  Multiply by the metrics of the second harmonic operator.  Save
!  into work array "LapT".
!
            DO j=J_RANGE
              DO i=I_RANGE
                cff=pm(i,j)*pn(i,j)
                cff1=1.0_r8/Hz(i,j,k)
                LapT(i,j,k)=cff1*(cff*                                  &
     &                            (FX(i+1,j)-FX(i,j)+                   &
     &                             FE(i,j+1)-FE(i,j))+                  &
     &                            (FS(i,j,k2)-FS(i,j,k1)))
              END DO
            END DO
          END IF
        END DO K_LOOP1
!
!  Apply boundary conditions (except periodic; closed or gradient)
!  to the first harmonic operator.
!
#ifndef EW_PERIODIC
        IF (WESTERN_EDGE) THEN
          DO k=1,N(ng)
            DO j=J_RANGE
# ifdef WESTERN_WALL
              LapT(Istr-1,j,k)=0.0_r8
# else
              LapT(Istr-1,j,k)=LapT(Istr,j,k)
# endif
            END DO
          END DO
        END IF
        IF (EASTERN_EDGE) THEN
          DO k=1,N(ng)
            DO j=J_RANGE
# ifdef EASTERN_WALL
              LapT(Iend+1,j,k)=0.0_r8
# else
              LapT(Iend+1,j,k)=LapT(Iend,j,k)
# endif
            END DO
          END DO
        END IF
#endif
#ifndef NS_PERIODIC
        IF (SOUTHERN_EDGE) THEN
          DO k=1,N(ng)
            DO i=I_RANGE
# ifdef SOUTHERN_WALL
              LapT(i,Jstr-1,k)=0.0_r8
# else
              LapT(i,Jstr-1,k)=LapT(i,Jstr,k)
# endif
            END DO
          END DO
        END IF
        IF (NORTHERN_EDGE) THEN
          DO k=1,N(ng)
            DO i=I_RANGE
# ifdef NORTHERN_WALL
              LapT(i,Jend+1,k)=0.0_r8
# else
              LapT(i,Jend+1,k)=LapT(i,Jend,k)
# endif
            END DO
          END DO
        END IF
#endif
#if !defined EW_PERIODIC && !defined NS_PERIODIC
        IF ((SOUTHERN_EDGE).and.(WESTERN_EDGE)) THEN
          DO k=1,N(ng)
            LapT(Istr-1,Jstr-1,k)=0.5_r8*(LapT(Istr  ,Jstr-1,k)+        &
     &                                    LapT(Istr-1,Jstr  ,k))
          END DO
        END IF
        IF ((SOUTHERN_EDGE).and.(EASTERN_EDGE)) THEN
          DO k=1,N(ng)
            LapT(Iend+1,Jstr-1,k)=0.5_r8*(LapT(Iend  ,Jstr-1,k)+        &
     &                                    LapT(Iend+1,Jstr  ,k))
          END DO
        END IF
        IF ((NORTHERN_EDGE).and.(WESTERN_EDGE)) THEN
          DO k=1,N(ng)
            LapT(Istr-1,Jend+1,k)=0.5_r8*(LapT(Istr  ,Jend+1,k)+        &
     &                                    LapT(Istr-1,Jend  ,k))
          END DO
        END IF
        IF ((NORTHERN_EDGE).and.(EASTERN_EDGE)) THEN
          DO k=1,N(ng)
            LapT(Iend+1,Jend+1,k)=0.5_r8*(LapT(Iend  ,Jend+1,k)+        &
     &                                    LapT(Iend+1,Jend  ,k))
          END DO
        END IF
#endif
!
!  Compute horizontal and density gradients associated with the
!  second rotated harmonic operator.
!
        k2=1
        K_LOOP2: DO k=0,N(ng)
          k1=k2
          k2=3-k1
          IF (k.lt.N(ng)) THEN
            DO j=Jstr,Jend
              DO i=Istr,Iend+1
                cff=0.5_r8*(pm(i,j)+pm(i-1,j))
#ifdef MASKING
                cff=cff*umask(i,j)
#endif
                dRdx(i,j,k2)=cff*(rho(i  ,j,k+1)-                       &
     &                            rho(i-1,j,k+1))
                dTdx(i,j,k2)=cff*(LapT(i  ,j,k+1)-                      &
     &                            LapT(i-1,j,k+1))
              END DO
            END DO
            DO j=Jstr,Jend+1
              DO i=Istr,Iend
                cff=0.5_r8*(pn(i,j)+pn(i,j-1))
#ifdef MASKING
                cff=cff*vmask(i,j)
#endif
                dRde(i,j,k2)=cff*(rho(i,j  ,k+1)-                       &
     &                            rho(i,j-1,k+1))
                dTde(i,j,k2)=cff*(LapT(i,j  ,k+1)-                      &
     &                            LapT(i,j-1,k+1))
              END DO
            END DO
          END IF
          IF ((k.eq.0).or.(k.eq.N(ng))) THEN
            DO j=Jstr-1,Jend+1
              DO i=Istr-1,Iend+1
                dTdr(i,j,k2)=0.0_r8
                FS(i,j,k2)=0.0_r8
              END DO
            END DO
          ELSE
            DO j=Jstr-1,Jend+1
              DO i=Istr-1,Iend+1
#if defined MAX_SLOPE
                cff1=SQRT(dRdx(i,j,k2)**2+dRdx(i+1,j,k2)**2+            &
     &                    dRdx(i,j,k1)**2+dRdx(i+1,j,k1)**2+            &
     &                    dRde(i,j,k2)**2+dRde(i,j+1,k2)**2+            &
     &                    dRde(i,j,k1)**2+dRde(i,j+1,k1)**2)
                cff2=0.25_r8*slope_max*                                 &
     &               (z_r(i,j,k+1)-z_r(i,j,k))*cff1
                cff3=MAX(rho(i,j,k)-rho(i,j,k+1),small)
                cff4=MAX(cff2,cff3)
                cff=-1.0_r8/cff4
#elif defined MIN_STRAT
                cff1=MAX(rho(i,j,k)-rho(i,j,k+1),                       &
     &                   strat_min*(z_r(i,j,k+1)-z_r(i,j,k)))
                cff=-1.0_r8/cff1
#else
                cff1=MAX(rho(i,j,k)-rho(i,j,k+1),eps)
                cff=-1.0_r8/cff1
#endif
                dTdr(i,j,k2)=cff*(LapT(i,j,k+1)-                        &
     &                            LapT(i,j,k  ))
                FS(i,j,k2)=cff*(z_r(i,j,k+1)-                           &
     &                          z_r(i,j,k  ))
              END DO
            END DO
          END IF
!
!  Compute components of the rotated tracer flux (T m4/s) along
!  isopycnic surfaces.
!
          IF (k.gt.0) THEN
            DO j=Jstr,Jend
              DO i=Istr,Iend+1
                cff=0.25_r8*(diff4(i,j,itrc)+diff4(i-1,j,itrc))*        &
     &              on_u(i,j)
                FX(i,j)=cff*                                            &
     &                  (Hz(i,j,k)+Hz(i-1,j,k))*                        &
     &                  (dTdx(i,j,k1)-                                  &
     &                   0.5_r8*(MAX(dRdx(i,j,k1),0.0_r8)*              &
     &                              (dTdr(i-1,j,k1)+                    &
     &                               dTdr(i  ,j,k2))+                   &
     &                           MIN(dRdx(i,j,k1),0.0_r8)*              &
     &                              (dTdr(i-1,j,k2)+                    &
     &                               dTdr(i  ,j,k1))))
              END DO
            END DO
            DO j=Jstr,Jend+1
              DO i=Istr,Iend
                cff=0.25_r8*(diff4(i,j,itrc)+diff4(i,j-1,itrc))*        &
     &              om_v(i,j)
                FE(i,j)=cff*                                            &
     &                  (Hz(i,j,k)+Hz(i,j-1,k))*                        &
     &                  (dTde(i,j,k1)-                                  &
     &                   0.5_r8*(MAX(dRde(i,j,k1),0.0_r8)*              &
     &                              (dTdr(i,j-1,k1)+                    &
     &                               dTdr(i,j  ,k2))+                   &
     &                           MIN(dRde(i,j,k1),0.0_r8)*              &
     &                              (dTdr(i,j-1,k2)+                    &
     &                               dTdr(i,j  ,k1))))
              END DO
            END DO
            IF (k.lt.N(ng)) THEN
              DO j=Jstr,Jend
                DO i=Istr,Iend
                  fac=0.5*diff4(i,j,itrc)
                  cff1=MAX(dRdx(i  ,j,k1),0.0_r8)
                  cff2=MAX(dRdx(i+1,j,k2),0.0_r8)
                  cff3=MIN(dRdx(i  ,j,k2),0.0_r8)
                  cff4=MIN(dRdx(i+1,j,k1),0.0_r8)
                  cff=cff1*(cff1*dTdr(i,j,k2)-dTdx(i  ,j,k1))+          &
     &                cff2*(cff2*dTdr(i,j,k2)-dTdx(i+1,j,k2))+          &
     &                cff3*(cff3*dTdr(i,j,k2)-dTdx(i  ,j,k2))+          &
     &                cff4*(cff4*dTdr(i,j,k2)-dTdx(i+1,j,k1))
                  cff1=MAX(dRde(i,j  ,k1),0.0_r8)
                  cff2=MAX(dRde(i,j+1,k2),0.0_r8)
                  cff3=MIN(dRde(i,j  ,k2),0.0_r8)
                  cff4=MIN(dRde(i,j+1,k1),0.0_r8)
                  cff=cff+                                              &
     &                cff1*(cff1*dTdr(i,j,k2)-dTde(i,j  ,k1))+          &
     &                cff2*(cff2*dTdr(i,j,k2)-dTde(i,j+1,k2))+          &
     &                cff3*(cff3*dTdr(i,j,k2)-dTde(i,j  ,k2))+          &
     &                cff4*(cff4*dTdr(i,j,k2)-dTde(i,j+1,k1))
                  FS(i,j,k2)=fac*cff*FS(i,j,k2)
                END DO
              END DO
            END IF
!
! Time-step biharmonic, isopycnal diffusion term (m Tunits).
!
            DO j=Jstr,Jend
              DO i=Istr,Iend
                cff=dt(ng)*pm(i,j)*pn(i,j)*                             &
     &                     (FX(i+1,j)-FX(i,j)+                          &
     &                      FE(i,j+1)-FE(i,j))+                         &
     &              dt(ng)*(FS(i,j,k2)-FS(i,j,k1))
                t(i,j,k,nnew,itrc)=t(i,j,k,nnew,itrc)-cff
#ifdef TS_MPDATA
                t(i,j,k,3,itrc)=t(i,j,k,nnew,itrc)
#endif
#ifdef DIAGNOSTICS_TS
                DiaTwrk(i,j,k,itrc,iThdif)=-cff
#endif
              END DO
            END DO
          END IF
        END DO K_LOOP2
      END DO T_LOOP
#undef I_RANGE
#undef J_RANGE
      RETURN
      END SUBROUTINE t3dmix4_tile

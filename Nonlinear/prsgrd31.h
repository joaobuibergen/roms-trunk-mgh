      SUBROUTINE prsgrd (ng, tile)
!
!****************************************** Alexander F. Shchepetkin ***
!  Copyright (c) 2002 ROMS/TOMS Group                                  !
!************************************************** Hernan G. Arango ***
!                                                                      !
!  This subroutine evaluates the  baroclinic  hydrostatic  pressure    !
!  gradient term using  the STANDARD density Jacobian  or  WEIGHTED    !
!  density Jacobian scheme of Song (1998). Both of these approaches    !
!  compute horizontal differences of density before of the vertical    !
!  integration.                                                        !
!                                                                      !
!  The pressure gradient terms (m4/s2) are loaded into right-hand-     !
!  side arrays "ru" and "rv".                                          !
!                                                                      !
!  Reference:                                                          !
!                                                                      !
!    Song, Y.T., 1998:  A general pressure gradient formulation for    !
!      numerical ocean models. Part I: Scheme design and diagnostic    !
!      analysis, Monthly Weather Rev., 126, 3213-3230.                 !
!                                                                      !
!***********************************************************************
!
      USE mod_param
# ifdef DIAGNOSTICS
      USE mod_diags
# endif
      USE mod_grid
      USE mod_ocean
      USE mod_stepping
!
      integer, intent(in) :: ng, tile

# include "tile.h"
!
# ifdef PROFILE
      CALL wclock_on (ng, 23)
# endif
      CALL prsgrd_tile (ng, Istr, Iend, Jstr, Jend,                     &
     &                  LBi, UBi, LBj, UBj,                             &
     &                  nrhs(ng),                                       &
     &                  GRID(ng) % Hz,                                  &
     &                  GRID(ng) % om_v,                                &
     &                  GRID(ng) % on_u,                                &
     &                  GRID(ng) % z_r,                                 &
     &                  GRID(ng) % z_w,                                 &
     &                  OCEAN(ng) % rho,                                &
# ifdef DIAGNOSTICS_UV
     &                  DIAGS(ng) % DiaRU,                              &
     &                  DIAGS(ng) % DiaRV,                              &
# endif
     &                  OCEAN(ng) % ru,                                 &
     &                  OCEAN(ng) % rv)
# ifdef PROFILE
      CALL wclock_off (ng, 23)
# endif
      RETURN
      END SUBROUTINE prsgrd
!
!***********************************************************************
      SUBROUTINE prsgrd_tile (ng, Istr, Iend, Jstr, Jend,               &
     &                        LBi, UBi, LBj, UBj,                       &
     &                        nrhs,                                     &
     &                        Hz, om_v, on_u, z_r, z_w,                 &
     &                        rho,                                      &
# ifdef DIAGNOSTICS_UV
     &                        DiaRU, DiaRV,                             &
# endif
     &                        ru, rv)
!***********************************************************************
!
      USE mod_param
      USE mod_scalars
!
!  Imported variable declarations.
!
      integer, intent(in) :: ng, Iend, Istr, Jend, Jstr
      integer, intent(in) :: LBi, UBi, LBj, UBj
      integer, intent(in) :: nrhs

# ifdef ASSUMED_SHAPE
      real(r8), intent(in) :: Hz(LBi:,LBj:,:)
      real(r8), intent(in) :: om_v(LBi:,LBj:)
      real(r8), intent(in) :: on_u(LBi:,LBj:)
      real(r8), intent(in) :: z_r(LBi:,LBj:,:)
      real(r8), intent(in) :: z_w(LBi:,LBj:,0:)
      real(r8), intent(in) :: rho(LBi:,LBj:,:)

#  ifdef DIAGNOSTICS_UV
      real(r8), intent(inout) :: DiaRU(LBi:,LBj:,:,:,:)
      real(r8), intent(inout) :: DiaRV(LBi:,LBj:,:,:,:)
#  endif
      real(r8), intent(inout) :: ru(LBi:,LBj:,0:,:)
      real(r8), intent(inout) :: rv(LBi:,LBj:,0:,:)
# else
      real(r8), intent(in) :: Hz(LBi:UBi,LBj:UBj,N(ng))
      real(r8), intent(in) :: om_v(LBi:UBi,LBj:UBj)
      real(r8), intent(in) :: on_u(LBi:UBi,LBj:UBj)
      real(r8), intent(in) :: z_r(LBi:UBi,LBj:UBj,N(ng))
      real(r8), intent(in) :: z_w(LBi:UBi,LBj:UBj,0:N(ng))
      real(r8), intent(in) :: rho(LBi:UBi,LBj:UBj,N(ng))

#  ifdef DIAGNOSTICS_UV
      real(r8), intent(inout) :: DiaRU(LBi:UBi,LBj:UBj,N(ng),2,NDrhs)
      real(r8), intent(inout) :: DiaRV(LBi:UBi,LBj:UBj,N(ng),2,NDrhs)
#  endif
      real(r8), intent(inout) :: ru(LBi:UBi,LBj:UBj,0:N(ng),2)
      real(r8), intent(inout) :: rv(LBi:UBi,LBj:UBj,0:N(ng),2)
# endif
!
!  Local variable declarations.
!
      integer :: IstrR, IendR, JstrR, JendR, IstrU, JstrV
      integer :: i, j, k
      real(r8) :: cff, cff1
# ifdef WJ_GRADP
      real(r8) :: gamma
# endif

      real(r8), dimension(PRIVATE_1D_SCRATCH_ARRAY) :: phie
      real(r8), dimension(PRIVATE_1D_SCRATCH_ARRAY) :: phix

# include "set_bounds.h"
!
!-----------------------------------------------------------------------
!  Calculate pressure gradient in the XI-direction (m4/s2).
!-----------------------------------------------------------------------
!
!  Compute surface baroclinic pressure gradient.
!
      DO j=Jstr,Jend
        cff=0.5_r8*g/rho0
        cff1=1000.0_r8*g/rho0
        DO i=IstrU,Iend
          phix(i)=cff*(rho(i,j,N(ng))-rho(i-1,j,N(ng)))*                &
     &            (z_w(i  ,j,N(ng))-z_r(i  ,j,N(ng))+                   &
     &             z_w(i-1,j,N(ng))-z_r(i-1,j,N(ng)))
# ifdef RHO_SURF
          phix(i)=phix(i)+                                              &
     &            (cff1+cff*(rho(i,j,N(ng))+rho(i-1,j,N(ng))))*         &
     &            (z_w(i,j,N(ng))-z_w(i-1,j,N(ng)))
# endif
          ru(i,j,N(ng),nrhs)=-0.5_r8*(Hz(i,j,N(ng))+Hz(i-1,j,N(ng)))*   &
     &                       phix(i)*on_u(i,j)
# ifdef DIAGNOSTICS_UV
          DiaRU(i,j,N(ng),nrhs,M3pgrd)=ru(i,j,N(ng),nrhs)
# endif
        END DO
!
!  Compute interior baroclinic pressure gradient.  Differentiate and
!  then vertically integrate.
!
        cff=0.25_r8*g/rho0
        DO k=N(ng)-1,1,-1
          DO i=IstrU,Iend
# ifdef WJ_GRADP
            gamma=0.125_r8*(z_r(i  ,j,k  )-z_r(i-1,j,k  )+              &
     &                      z_r(i  ,j,k+1)-z_r(i-1,j,k+1))*             &
     &                     (z_r(i  ,j,k+1)-z_r(i  ,j,k  )-              &
     &                      z_r(i-1,j,k+1)+z_r(i-1,j,k  ))/             &
     &            ((z_r(i  ,j,k+1)-z_r(i  ,j,k))*                       &
     &             (z_r(i-1,j,k+1)-z_r(i-1,j,k)))
# endif
            phix(i)=phix(i)+cff*                                        &
# ifdef WJ_GRADP
     &              (((1.0_r8+gamma)*(rho(i,j,k+1)-rho(i-1,j,k+1))+     &
     &                (1.0_r8-gamma)*(rho(i,j,k  )-rho(i-1,j,k  )))*    &
     &               (z_r(i,j,k+1)+z_r(i-1,j,k+1)-                      &
     &                z_r(i,j,k  )-z_r(i-1,j,k  ))                      &
     &              -(rho(i,j,k+1)+rho(i-1,j,k+1)-                      &
     &                rho(i,j,k  )-rho(i-1,j,k  ))*                     &
     &               ((1.0_r8+gamma)*(z_r(i,j,k+1)-z_r(i-1,j,k+1))+     &
     &                (1.0_r8-gamma)*(z_r(i,j,k  )-z_r(i-1,j,k  ))))
# else
     &              ((rho(i,j,k+1)-rho(i-1,j,k+1)+                      &
     &                rho(i,j,k  )-rho(i-1,j,k  ))*                     &
     &               (z_r(i,j,k+1)+z_r(i-1,j,k+1)-                      &
     &                z_r(i,j,k  )-z_r(i-1,j,k  ))                      &
     &              -(rho(i,j,k+1)+rho(i-1,j,k+1)-                      &
     &                rho(i,j,k  )-rho(i-1,j,k  ))*                     &
     &               (z_r(i,j,k+1)-z_r(i-1,j,k+1)+                      &
     &                z_r(i,j,k  )-z_r(i-1,j,k  )))
# endif
            ru(i,j,k,nrhs)=-0.5_r8*(Hz(i,j,k)+Hz(i-1,j,k))*             &
     &                     phix(i)*on_u(i,j)
# ifdef DIAGNOSTICS_UV
            DiaRU(i,j,k,nrhs,M3pgrd)=ru(i,j,k,nrhs)
# endif
          END DO
        END DO
!
!-----------------------------------------------------------------------
!  Calculate pressure gradient in the ETA-direction (m4/s2).
!-----------------------------------------------------------------------
!
!  Compute surface baroclinic pressure gradient.
!
        IF (j.ge.JstrV) THEN
          cff=0.5_r8*g/rho0
          cff1=1000.0_r8*g/rho0
          DO i=Istr,Iend
            phie(i)=cff*(rho(i,j,N(ng))-rho(i,j-1,N(ng)))*              &
     &              (z_w(i,j  ,N(ng))-z_r(i,j  ,N(ng))+                 &
     &               z_w(i,j-1,N(ng))-z_r(i,j-1,N(ng)))
# ifdef RHO_SURF
            phie(i)=phie(i)+                                            &
     &              (cff1+cff*(rho(i,j,N(ng))+rho(i,j-1,N(ng))))*       &
     &              (z_w(i,j,N(ng))-z_w(i,j-1,N(ng)))
# endif
            rv(i,j,N(ng),nrhs)=-0.5_r8*(Hz(i,j,N(ng))+Hz(i,j-1,N(ng)))* &
     &                         phie(i)*om_v(i,j)
# ifdef DIAGNOSTICS_UV
            DiaRV(i,j,N(ng),nrhs,M3pgrd)=rv(i,j,N(ng),nrhs)
# endif
          END DO
!
!  Compute interior baroclinic pressure gradient.  Differentiate and
!  then vertically integrate.
!
          cff=0.25_r8*g/rho0
          DO k=N(ng)-1,1,-1
            DO i=Istr,Iend
# ifdef WJ_GRADP
              gamma=0.125_r8*(z_r(i,j  ,k  )-z_r(i,j-1,k  )+            &
     &                        z_r(i,j  ,k+1)-z_r(i,j-1,k+1))*           &
     &                       (z_r(i,j  ,k+1)-z_r(i,j  ,k  )-            &
     &                        z_r(i,j-1,k+1)+z_r(i,j-1,k  ))/           &
     &              ((z_r(i,j  ,k+1)-z_r(i,j  ,k))*                     &
     &               (z_r(i,j-1,k+1)-z_r(i,j-1,k)))
# endif
              phie(i)=phie(i)+cff*                                      &
# ifdef WJ_GRADP
     &                (((1.0_r8+gamma)*(rho(i,j,k+1)-rho(i,j-1,k+1))+   &
     &                  (1.0_r8-gamma)*(rho(i,j,k  )-rho(i,j-1,k  )))*  &
     &                 (z_r(i,j,k+1)+z_r(i,j-1,k+1)-                    &
     &                  z_r(i,j,k  )-z_r(i,j-1,k  ))                    &
     &                -(rho(i,j,k+1)+rho(i,j-1,k+1)-                    &
     &                  rho(i,j,k  )-rho(i,j-1,k  ))*                   &
     &                 ((1.0_r8+gamma)*(z_r(i,j,k+1)-z_r(i,j-1,k+1))+   &
     &                  (1.0_r8-gamma)*(z_r(i,j,k  )-z_r(i,j-1,k  ))))
# else
     &                ((rho(i,j,k+1)-rho(i,j-1,k+1)+                    &
     &                  rho(i,j,k  )-rho(i,j-1,k  ))*                   &
     &                 (z_r(i,j,k+1)+z_r(i,j-1,k+1)-                    &
     &                  z_r(i,j,k  )-z_r(i,j-1,k  ))                    &
     &                -(rho(i,j,k+1)+rho(i,j-1,k+1)-                    &
     &                  rho(i,j,k  )-rho(i,j-1,k  ))*                   &
     &                 (z_r(i,j,k+1)-z_r(i,j-1,k+1)+                    &
     &                  z_r(i,j,k  )-z_r(i,j-1,k  )))
# endif
              rv(i,j,k,nrhs)=-0.5_r8*(Hz(i,j,k)+Hz(i,j-1,k))*           &
     &                       phie(i)*om_v(i,j)
# ifdef DIAGNOSTICS_UV
              DiaRV(i,j,k,nrhs,M3pgrd)=rv(i,j,k,nrhs)
# endif
            END DO
          END DO
        END IF
      END DO
      RETURN
      END SUBROUTINE prsgrd_tile

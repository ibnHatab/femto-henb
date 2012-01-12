                                 fapi
                                 ====

Author: Vlad K.
Date: 2012-01-12 12:54:20 EET


Table of Contents
=================
1 Interfaces
    1.1 LTE MAC Scheduler Interface Specification v1.11
        1.1.1 Interface definition for CSCHED SAP and SCHED SAP with Properties <2012-01-12 Thu>
    1.2 LTE eNB L1 API Definition v1.11
    1.3 LTE Network Monitor Mode Specification v1.01


1 Interfaces
~~~~~~~~~~~~~
  Several APIs are defined, as follows:
  P1 – the Security coprocessor interface
  P2 – the Service Discovery Interface
  P3 – the GPS interface
  P4 – the Network Listen Results interface
  P5 – the PHY mode control interface
  P6 – the ciphering coprocessor interface
  P7 – the main datapath interface
  M1 – the scheduler interface

1.1 LTE MAC Scheduler Interface Specification v1.11 :DOCS:
==========================================================
   October 2010

   Index number: FF_Tech_001_v1.11

   This document specifies the MAC Scheduler interface. The goal of this
   interface specification is to allow the use of a wide range of
   schedulers which can be plugged into the eNodeB and to allow for
   standardized interference coordination interface to the scheduler.

1.1.1 DONE Interface definition for CSCHED SAP and SCHED SAP with Properties <2012-01-12 Thu> :CODING:
------------------------------------------------------------------------------------------------------

1.2 LTE eNB L1 API Definition v1.11
====================================
   October 2010

   Index number: FF_Tech_002_v1.11

   This document describes an Application Programming Interface (API)
   which defines the interface between LTE L2/L3 software and L1
   PHY. Specifically, this L1 API defines both P5 and P7 of the Femto
   Forum LTE FAPI.

1.3 LTE Network Monitor Mode Specification v1.01
=================================================

   October 2010

   Index number: FF_Tech_003_v1.01

   This document specifies the Network Monitor Mode (NMM) module for
   LTE. Support for 3G and 2G is not in the scope of this document. The
   goal of this interface specification is to allow a femtocell to
   support cell RF self configuration and interference management
   according to requirements which, amongst others, include: carrier and
   cell id selection; DL TX power setting; UE TX power setting and
   frequency reference.


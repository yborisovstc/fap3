MyRoot : Elem {
    # "Test of iteration reset"
    # "Interator resets each time the input changed,"
    # "Data change detection method is used to get -reset-, ref ds_dcs_iui_dci"
    # "So it completes iteration as soon as inputs get stable."
    + DesUtils
    Launcher : DesLauncher {
        Debug.LogLevel = "Dbg"
        S1 : State {
            = "SI 3"
        }
        S3 : State {
            = "SI 5"
        }
        S2 : State @  {
            _@ <  {
                Debug.LogLevel = "Dbg"
                = "SI 1"
            }
            Inp ~ : TrSwitchBool @  {
                Inp1 ~ S2
                Inp2 ~ : TrAddVar @  {
                    Inp ~ S2
                    Inp ~ : State {
                        = "SI 1"
                    }
                }
                Sel ~ Cmp1_Lt : TrCmpVar @  {
                    Inp ~ S2
                    Inp2 ~ : State {
                        = "SI 10"
                    }
                }
            }
        }
        S4 : State {
            = "SI 4"
        }
        Ext : ExtdStateOutp @  {
            Int ~ S1
            Int ~ S2
            Int ~ S3
        }
        # "Data change detector, ref ds_dcs_iui_tgh"
        ChgDet : DesUtils.ChgDetector @  {
            Inp ~ Ext
        }
        # "Intputs iterator"
        InpIterator : DesUtils.InpItr @  {
            InpM ~ Ext
            InpDone ~ : State {
                = "SB true"
            }
            InpReset ~ ChgDet.Outp
            InpReset_Dbg : State @  {
                _@ <  {
                    Debug.LogLevel = "Dbg"
                    = "SB _INV"
                }
                Inp ~ ChgDet.Outp
            }
        }
        Iter_Dbg : State @  {
            _@ <  {
                Debug.LogLevel = "Dbg"
                = "SI _INV"
            }
            Inp ~ InpIterator.Outp
        }
        Iter_Done_Dbg : State @  {
            _@ <  {
                Debug.LogLevel = "Dbg"
                = "SB _INV"
            }
            Inp ~ InpIterator.OutpDone
        }
        Max : State @  {
            _@ <  {
                Debug.LogLevel = "Dbg"
                = "SI -1"
            }
            Inp ~ : TrSwitchBool @  {
                Debug.LogLevel = "Dbg"
                Inp1 ~ Max
                Inp2 ~ : TrInpSel @  {
                    Inp ~ Ext
                    Idx ~ InpIterator.Outp
                }
                Sel ~ Max_Lt : TrCmpVar @  {
                    Debug.LogLevel = "Dbg"
                    Inp ~ Max
                    Inp2 ~ : TrInpSel @  {
                        Debug.LogLevel = "Dbg"
                        Inp ~ Ext
                        Idx ~ InpIterator.Outp
                    }
                }
            }
        }
        IdxOfMax : State @  {
            _@ <  {
                Debug.LogLevel = "Dbg"
                = "SI -1"
            }
            Inp ~ : TrSwitchBool @  {
                Inp1 ~ IdxOfMax
                Inp2 ~ InpIterator.Outp
                Sel ~ Max_Lt
            }
        }
    }
}

MyRoot : Elem {
    # "Test of iteration reset"
    # "Interator resets each time the input changed,"
    # "So it completes iteration as soon as inputs get stable."
    + DesUtils
    Launcher : DesLauncher {
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
        Ext2 : ExtdStateOutp @  {
            Int ~ S1
            Int ~ S2
            Int ~ S3
        }
        As : DesAs {
            Ext : ExtdStateOutp
            # "Changes detector"
            ChgDetector_Neq : TrCmpVar @  {
                Inp ~ StInpSig : State @  {
                    _@ <  {
                        Debug.LogLevel = "Dbg"
                        = "SI -1"
                    }
                    Inp ~ InpSig : TrAddVar @  {
                        Inp ~ Ext
                    }
                }
                Inp2 ~ InpSig
            }
            ChgDetector_Dbg : State @  {
                _@ <  {
                    Debug.LogLevel = "Dbg"
                    = "SB _INV"
                }
                Inp ~ ChgDetector_Neq
            }
            # "Intputs iterator"
            InpIterator : DesUtils.InpItr @  {
                InpM ~ Ext
                InpDone ~ : State {
                    = "SB true"
                }
                InpReset ~ ChgDetector_Neq
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
                    Inp1 ~ Max
                    Inp2 ~ : TrInpSel @  {
                        Inp ~ Ext
                        Idx ~ InpIterator.Outp
                    }
                    Sel ~ Max_Lt : TrCmpVar @  {
                        Inp ~ Max
                        Inp2 ~ : TrInpSel @  {
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
        As.Ext.Int ~ Ext2
    }
}

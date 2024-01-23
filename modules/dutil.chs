DesUtils : Elem {
    # "DES utilities"
    BChange : Des {
        # "Boolean change"
        SInp : Extd {
            Int : CpStateOutp
        }
        Outp : Extd {
            Int : CpStateInp
        }
        TAnd : TrAndVar (
            Inp ~ SInp.Int
            Inp ~ : TrNegVar (
                Inp ~ Delay : State (
                    _@ < = "SB false"
                    _@ < Debug.LogLevel = "Dbg"
                    Inp ~ SInp.Int
                )
            )
        )
        Outp.Int ~ TAnd
    }
    BChangeCnt : Des {
        # "Boolean change counter"
        SInp : Extd {
            Int : CpStateOutp
        }
        Outp : Extd {
            Int : CpStateInp
        }
        Chg : BChange
        Chg.SInp ~ SInp.Int
        Cnt : State (
            _@ < = "SI 0"
            _@ < Debug.LogLevel = "Dbg"
            Inp ~ : TrAddVar (
                Inp ~ Cnt
                Inp ~ : TrSwitchBool (
                    Sel ~ Chg.Outp
                    Inp1 ~ Const_0 : State {
                        = "SI 0"
                    }
                    Inp2 ~ Const_1 : State {
                        = "SI 1"
                    }
                )
            )
        )
        Outp.Int ~ Cnt
    }
    SetTg : Des {
        # "Set trigger"
        InpSet : Extd {
            Int : CpStateOutp
        }
        Outp : Extd {
            Int : CpStateInp
        }
        Outp.Int ~ Value : State (
            _@ <  {
                Debug.LogLevel = "Dbg"
                = "SB false"
            }
            Inp ~ : TrOrVar (
                Inp ~ InpSet.Int
                Inp ~ Value
            )
        )
    }
    RSTg : Des {
        # "R/S trigger, positive inputs"
        InpS : Extd {
            Int : CpStateOutp
        }
        InpR : Extd {
            Int : CpStateOutp
        }
        Outp : Extd {
            Int : CpStateInp
        }
        Outp.Int ~ Value : State (
            _@ <  {
                Debug.LogLevel = "Dbg"
                = "SB false"
            }
            Inp ~ : TrOrVar (
                Inp ~ InpS.Int
                Inp ~ : TrAndVar (
                    Inp ~ : TrNegVar (
                        Inp ~ InpR.Int
                    )
                    Inp ~ Value
                )
            )
        )
    }
    DPulse : Des {
        # "Data pulse"
        # "Set delay and const type before usage"
        # "InpD - data input, InpE - data to pass when no changes happen"
        InpD : Extd {
            Int : CpStateOutp
        }
        InpE : Extd {
            Int : CpStateOutp
        }
        Outp : Extd {
            Int : CpStateInp
        }
        Outp.Int ~ : TrSwitchBool (
            Sel ~ Cmp_Neq : TrCmpVar (
                Inp ~ InpD.Int
                Inp2 ~ Delay : State (
                    Inp ~ InpD.Int
                )
            )
            Inp1 ~ InpE.Int
            Inp2 ~ InpD.Int
        )
    }
    IdxItr : Des {
        # "Index based iterator"
        # "InpCnt - container elements count"
        # "InpDone - sign of selected input is handled"
        # "OutpDone - sign of iterator reaches the end"
        InpCnt : ExtdStateInp
        InpDone : ExtdStateInp
        InpReset : ExtdStateInp
        Outp : ExtdStateOutp
        OutpDone : ExtdStateOutp
        ICnt_Dbg : State (
            _@ <  {
                Debug.LogLevel = "Dbg"
                = "SI _INV"
            }
            Inp ~ InpCnt.Int
        )
        SIdx : State (
            # "Index"
            _@ <  {
                = "SI 0"
                Debug.LogLevel = "Dbg"
            }
            Inp ~ Sw1 : TrSwitchBool (
                Inp1 ~ Sw2 : TrSwitchBool (
                    Sel ~ CidxAnd1 : TrAndVar (
                        Inp ~ Cmp_Gt : TrCmpVar (
                            Inp ~ : TrAddVar (
                                Inp ~ InpCnt.Int
                                InpN ~ : State {
                                    = "SI 1"
                                }
                            )
                            Inp2 ~ SIdx
                            _@ < Debug.LogLevel = "Err"
                        )
                        Inp ~ InpDone.Int
                    )
                    Inp1 ~ SIdx
                    Inp2 ~ : TrAddVar (
                        Inp ~ SIdx
                        Inp ~ : State {
                            = "SI 1"
                        }
                    )
                )
                Inp2 ~ : State {
                    = "SI 0"
                }
                Sel ~ InpReset.Int
            )
        )
        Outp.Int ~ SIdx
        OutpDone.Int ~ : TrAndVar (
            Inp ~ InpDone.Int
            Inp ~ : TrNegVar (
                Inp ~ Cmp_Gt
            )
        )
    }
    IdxItr2 : Des {
        # "Index based iterator"
        # "Ver.2 Set outp to invalid if container is empty"
        # "InpCnt - container elements count"
        # "InpDone - sign of selected input is handled"
        # "OutpDone - sign of iterator reaches the end"
        InpCnt : ExtdStateInp
        InpDone : ExtdStateInp
        InpReset : ExtdStateInp
        Outp : ExtdStateOutp
        OutpDone : ExtdStateOutp
        ICnt_Dbg : State (
            _@ <  {
                Debug.LogLevel = "Dbg"
                = "SI _INV"
            }
            Inp ~ InpCnt.Int
        )
        SIdx : State (
            # "Index"
            _@ <  {
                = "SI 0"
                Debug.LogLevel = "Dbg"
            }
            Inp ~ Sw1 : TrSwitchBool (
                Inp1 ~ Sw2 : TrSwitchBool (
                    Sel ~ CidxAnd1 : TrAndVar (
                        Inp ~ Cmp_Gt : TrCmpVar (
                            Inp ~ : TrAddVar (
                                Inp ~ InpCnt.Int
                                InpN ~ : State {
                                    = "SI 1"
                                }
                            )
                            Inp2 ~ SIdx
                            _@ < Debug.LogLevel = "Err"
                        )
                        Inp ~ InpDone.Int
                    )
                    Inp1 ~ SIdx
                    Inp2 ~ : TrAddVar (
                        Inp ~ SIdx
                        Inp ~ : State {
                            = "SI 1"
                        }
                    )
                )
                Inp2 ~ : State {
                    = "SI 0"
                }
                Sel ~ InpReset.Int
            )
        )
        # "Set outp to invalid if container is empty"
        Outp.Int ~ : TrSwitchBool (
            Inp1 ~ SIdx
            Inp2 ~ : Const {
                = "SI"
            }
            Sel ~ Cmp2_Eq : TrCmpVar (
                Inp ~ InpCnt.Int
                Inp2 ~ : SI_0
            )
        )
        OutpDone.Int ~ : TrAndVar (
            Inp ~ InpDone.Int
            Inp ~ : TrNegVar (
                Inp ~ Cmp_Gt
            )
        )
    }
    InpItr : Des {
        # "Inputs iterator"
        # "InpM - multiplexed input"
        # "InpDone - sign of selected input is handled"
        # "OutpDone - sign of iterator reaches the end"
        InpM : ExtdStateInp
        InpDone : ExtdStateInp
        InpReset : ExtdStateInp
        Outp : ExtdStateOutp
        OutpDone : ExtdStateOutp
        Itr : IdxItr (
            InpCnt ~ : TrInpCnt (
                Inp ~ InpM.Int
            )
        )
        Itr.InpDone ~ InpDone.Int
        Itr.InpReset ~ InpReset.Int
        Outp.Int ~ Itr.Outp
        OutpDone.Int ~ Itr.OutpDone
    }
    VectIter : IdxItr {
        # "Vector iterator"
        # "InpV - input (vector)"
        # "OutV - output - vector element"
        InpV : ExtdStateInp
        InpCnt ~ : TrSizeVar (
            Inp ~ InpV.Int
        )
        OutV : TrAtgVar (
            Inp ~ InpV.Int
            Index ~ Outp
        )
    }
    # "Data change detector, ref ds_dcs_iui_dci"
    ChgDetector : Des {
        Inp : ExtdStateInp
        Outp : ExtdStateOutp
        Outp.Int ~ Cmp_Neq : TrCmpVar (
            Cmp_Neq.Inp ~ StInpSig : State (
                _@ <  {
                    Debug.LogLevel = "Err"
                    = "SI _INV"
                }
                StInpSig.Inp ~ Hash : TrHash (
                    Hash.Inp ~ Inp.Int
                )
            )
            Cmp_Neq.Inp2 ~ Hash
        )
    }
    ListItemByPos : DesAs {
        # "DES active subsystem. Getting list node with given pos in the list"
        # "Input: node position in the list"
        InpPos : ExtdStateInp
        # "Input: link to observed list"
        InpMagLink : Link {
            Outp : CpStateMnodeOutp
        }
        # "Output: URI of list node with given pos"
        OutpNode : ExtdStateOutp
        Debug.LogLevel = "Err"
        Subsys : DAdp {
            InpPos : ExtdStateInp
            # "Desas init"
            Init : State {
                = "SB false"
                Debug.LogLevel = "Err"
            }
            CurPos : State (
                _@ <  {
                    Debug.LogLevel = "Err"
                    = "SI 0"
                }
                Inp ~ : TrSwitchBool (
                    Inp1 ~ : TrSwitchBool (
                        Inp1 ~ : TrAddVar (
                            Inp ~ CurPos
                            Inp ~ : State {
                                = "SI 1"
                            }
                        )
                        Inp2 ~ CurPos
                        Sel ~ PosReached_Ge : TrCmpVar (
                            Inp ~ CurPos
                            Inp2 ~ InpPos.Int
                        )
                    )
                    Inp2 ~ : State {
                        = "SI 0"
                    }
                    Sel ~ Init
                )
            )
            Res : State {
                = "URI _INV"
                Debug.LogLevel = "Err"
            }
            PairOfPrev : SdoTcPair (
                Targ ~ Res
                TargComp ~ : State {
                    = "URI Prev"
                }
            )
            PairOfPrev_Dbg : State (
                _@ <  {
                    Debug.LogLevel = "Err"
                    = "URI _INV"
                }
                Inp ~ PairOfPrev
            )
            PairOfPrevOwner : SdoCompOwner (
                Comp ~ PairOfPrev
            )
            Res.Inp ~ : TrSwitchBool (
                Inp1 ~ : TrSwitchBool (
                    Inp1 ~ PairOfPrevOwner
                    Inp2 ~ Res
                    Sel ~ PosReached_Ge
                )
                Inp2 ~ : State {
                    = "URI Start"
                }
                Sel ~ Init
            )
        }
        Subsys.InpPos ~ InpPos.Int
        Subsys.InpMagBase ~ InpMagLink.Outp
        Subsys.InpMagUri ~ : State {
            = "URI _$"
        }
        OutpNode.Int ~ Subsys.Res
        # "<<< DES active subsystem. Getting node with given pos of list"
    }
    # "TODO move to specific utils?"
    PrntMappingResolver : DesAs {
        # ">>> Parent mapping resolver"
        # "Finds data (URI) assosiated to parent"
        # "Input: parents hierarchy (VDU)"
        InpParents : ExtdStateInp
        # "Input: Mapping parent to result"
        InpMpg : ExtdStateInp
        # "Input: Default result"
        InpDefRes : ExtdStateInp
        # "Output: Resolved CRP URI"
        OutpRes : ExtdStateOutp
        Subsys : Des {
            InpSsParents : ExtdStateInp
            InpMapping : ExtdStateInp
            Outp : ExtdStateOutp
            # "Desas init indicator"
            Init : State {
                = "SB false"
                Debug.LogLevel = "Err"
            }
            ParentsIter : DesUtils.VectIter (
                _@ < Debug.LogLevel = "Dbg"
                InpV ~ InpSsParents.Int
                _ < InpDone ~ : SB_True
                InpReset ~ Init
            )
            FindCrp : TrFindByP (
                Inp ~ InpMapping.Int
                Sample ~ ParentsIter.OutV
            )
            ParentsIter.InpDone ~ : TrNegVar (
                Inp ~ : TrIsValid (
                    Inp ~ FindCrp
                )
            )
            Outp.Int ~ FindCrp
        }
        Parents_Dbg : State (
            _@ < Debug.LogLevel = "Dbg"
            _@ < = "VDU"
            Inp ~ InpParents.Int
        )
        Res_Dbg : State (
            _@ < Debug.LogLevel = "Dbg"
            _@ < = "URI"
            Inp ~ Subsys.Outp
        )
        Subsys.InpSsParents ~ InpParents.Int
        Subsys.InpMapping ~ InpMpg.Int
        OutpRes.Int ~ Subsys.Outp
        # ">>> Parent mapping resolver"
    }
}

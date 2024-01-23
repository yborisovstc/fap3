testroot : Elem {
    test : DesLauncher {
        Debug.LogLevel = "Dbg"
        # "MNode adapter test: observation of compoments number"
        # "Managed agent is not adapter owned - using input to access managed agent top owner"
        Controller : Des {
            Elem1 : Elem
            Elem2 : Elem1
            Elem3 : Elem2
            Targets : Syst {
                # "Target_1"
                Target : Node {
                    Cmp_0 : Node
                    Cmp_1 : Node
                }
                # "Target_2"
                Target2 : Unit {
                    Cmp2_0 : Node
                    Cmp2_1 : Node
                    Cmp2_2 : Node
                }
                # "Target_3"
                Target3 : Elem3 {
                    Cmp3_0 : Node
                    Cmp3_1 : Node
                    Cmp3_2 : Node
                    Cmp3_3 : Node
                }
            }
            # "Controller uses adapter for access to target"
            Adapter : DAdp {
                Debug.LogLevel = "Dbg"
                CompsCount : SdoCompsCount
                CompNames : SdoCompsNames
                Name : SdoName
                Parent : SdoParent
                Parents : SdoParents
                AddComp : ASdcComp {
                    Debug.LogLevel = "Dbg"
                }
            }
            TargBaseLink : Link {
                Outp : CpStateMnodeOutp
            }
            TargBaseLink ~ Targets
            Adapter.InpMagBase ~ TargBaseLink.Outp
            Adapter < AgentUri : Content {
                = "Target"
            }
            # "Components_count"
            CompCount : State {
                Debug.LogLevel = "Dbg"
                = "SI 0"
            }
            CompCount.Inp ~ Adapter.CompsCount
            # "Components"
            CompNames : State {
                Debug.LogLevel = "Dbg"
                = "VS"
            }
            CompNames.Inp ~ Adapter.CompNames
            # "Name"
            Name_Dbg : State (
                _@ < Debug.LogLevel = "Dbg"
                _@ < = "SS"
                Inp ~ Adapter.Name
            )
            # "Parent"
            Parent_Dbg : State (
                _@ < Debug.LogLevel = "Dbg"
                _@ < = "SS"
                Inp ~ Adapter.Parent
            )
            # "Parents"
            Parents_Dbg : State (
                _@ < Debug.LogLevel = "Dbg"
                _@ < = "VDU"
                Inp ~ Adapter.Parents
            )

            # "OutpMagUri debug"
            OutpMagUri_Dbg : State (
                _@ < Debug.LogLevel = "Dbg"
                _@ < = "SS"
                Inp ~ Adapter.OutpMagUri
            )
            # "Tics_Counter"
            Counter : State {
                Debug.LogLevel = "Dbg"
                = "SI 0"
            }
            Incr : TrAddVar
            Counter.Inp ~ Incr
            Const_1 : State {
                = "SI 1"
            }
            Incr.Inp ~ Const_1
            Incr.Inp ~ Counter
            # "Managed agent 1 URI"
            MagUri : State {
                = "SS Target2"
            }
            # "Managed agent 2 URI"
            MagUri2 : State {
                = "SS Target3"
            }
            # "Target switcher"
            Const_3 : State {
                = "SI 3"
            }
            Sw : TrSwitchBool (
                _@ < Debug.LogLevel = "Dbg"
                Sel ~ Cmp_Ge : TrCmpVar (
                    Inp ~ Counter
                    Inp2 ~ Const_3
                )
                Inp1 ~ MagUri
                Inp2 ~ MagUri2
            )
            Adapter.InpMagUri ~ : TrToUriVar (
                Inp ~ Sw
            )
            # "Mutating: adding component"
            Adapter.AddComp.Name ~ : State {
                = "SS New_node"
            }
            Adapter.AddComp.Parent ~ : State {
                = "SS Node"
            }
            Adapter.AddComp.Enable ~ Cmp2_Eq : TrCmpVar (
                Inp ~ Counter
                Inp2 ~ : State {
                    = "SI 6"
                }
            )
            AddCompOutp_Dbg : State (
                _@ <  {
                    Debug.LogLevel = "Dbg"
                    = "SB"
                }
                Inp ~ Adapter.AddComp.Outp
            )
        }
    }
}

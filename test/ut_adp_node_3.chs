testroot : Elem
{
    Modules : Node
    {
        + AdpComps;
    }
    test : DesLauncher
    {
        # "MNode adapter test: observation of compoments number";
        # "Managed agent is not adapter owned - using input to access managed agent top owner";
        Controller : Des
        {
            Targets : Node {
                # "Target_1";
                Target : Node {
                    Cmp_0 : Node;
                    Cmp_1 : Node;
                }
               # "Target_2";
               Target2 : Node {
                   Cmp2_0 : Node;
                   Cmp2_1 : Node;
                   Cmp2_2 : Node;
               }
               # "Target_3";
               Target3 : Node {
                   Cmp3_0 : Node;
                   Cmp3_1 : Node;
                   Cmp3_2 : Node;
                   Cmp3_3 : Node;
               }
            }
            # "Controller using adapter for access to target";
            Adapter : AdpComps.NodeAdp;
            TargBaseLink : Link {
                Outp : CpStateMnodeOutp;
            }
            TargBaseLink ~ Targets;
            Adapter.InpMagBase ~ TargBaseLink.Outp;
            Adapter < AgentUri : Content { = "Target"; }
            # "Components_count";
            CompCount : State {
                Debug.LogLevel = "Dbg";
                = "SI 0";
            }
            CompCount.Inp ~ Adapter.CompsCount;
            # "Components";
            CompNames : State {
                Debug.LogLevel = "Dbg";
                = "VS";
            }
            CompNames.Inp ~ Adapter.CompNames;
            # "Tics_Counter";
            Counter : State {
                Debug.LogLevel = "Dbg";
                = "SI 0";
            }
            Incr : TrAddVar;
            Counter.Inp ~ Incr;
            Const_1 : State {
                = "SI 1";
            }
            Incr.Inp ~ Const_1;
            Incr.Inp ~ Counter;
            # "Managed agent 1 URI";
            MagUri : State {
                Debug.LogLevel = "Dbg";
                = "SS Target2";
            }
            # "Managed agent 2 URI";
            MagUri2 : State {
                Debug.LogLevel = "Dbg";
                = "SS Target3";
            }
            # "Target switcher";
            Const_3 : State;
            Const_3 < = "SI 3";
            Cmp_Ge : TrCmpVar;
            Cmp_Ge.Inp ~ Counter;
            Cmp_Ge.Inp2 ~ Const_3;
            Sw : TrSwitchBool;
            Sw.Sel ~ Cmp_Ge;
            Sw.Inp1 ~ MagUri;
            Sw.Inp2 ~ MagUri2;
            Adapter.InpMagUri ~ Sw;
        }
    }
}

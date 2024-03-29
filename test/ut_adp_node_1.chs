testroot : Elem
{
    Modules : Node
    {
        + AdpComps;
    }
    test : DesLauncher
    {
        # "MNode adapter test: observation of compoments number";
        Controller : Des
        {
            # "Controller using adapter for access to target";
            Adapter : AdpComps.NodeAdp
            {
                # "Keeping target within Adapter tree at the moment, ref issue DS_SN_AUL";
                Targets : Node {
                    # "Target_1";
                   Target : Node;
                   Target < Cmp_0 : Node;
                   Target < Cmp_1 : Node;
                   # "Target_2";
                   Target2 : Node;
                   Target2 < Cmp2_0 : Node;
                   Target2 < Cmp2_1 : Node;
                   Target2 < Cmp2_2 : Node;
                   # "Target_3";
                   Target3 : Node;
                   Target3 < Cmp3_0 : Node;
                   Target3 < Cmp3_1 : Node;
                   Target3 < Cmp3_2 : Node;
                   Target3 < Cmp3_3 : Node;
                }
                AgentUri : Content { = "Targets.Target"; }
                SelfAsMagBase = "y";
            }
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
            # "Name";
            Name : State {
                Debug.LogLevel = "Dbg";
                = "SS";
            }
            Name.Inp ~ Adapter.Name;
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
                = "SS Targets.Target2";
            }
            # "Managed agent 2 URI";
            MagUri2 : State {
                Debug.LogLevel = "Dbg";
                = "SS Targets.Target3";
            }
            # "MagUri output";
            Dbg_MagUriOutp : State @ {
                _@ < Debug.LogLevel = "Dbg";
                _@ < = "SS ";
                Inp ~ Adapter.OutpMagUri;
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

Root : Elem {
    # "Test of SDC, connect and disconnect in cycle";
    Modules : Node
    {
    } 
    # "Runner";
    Launcher :  DesLauncher {
        Controllable = "y";
        # "Model";
        SConst_1 : State { = "SI 1"; }
        SConst_2 : State { = "SI 2"; }
        Sock1 : Socket { Inp : CpStateInp; }
        SResult : Sock1 @ {
            _@ < { Debug.LogLevel = "Dbg"; = "SI 0"; }
            Inp ~ : TrAddVar @ {
                Inp ~ : TrAddVar @ {
                    Inp ~ TrAdd : TrAddVar @ { Inp ~ SConst_1; };
                };
                Inp ~ TrAdd;
            };
        }
        # "Connect";
        Dc_Conn : ASdcConn @ {
            _@ < Debug.LogLevel = "Dbg";
            V1 ~ : State { = "SS SResult.Inp"; };
            V2 ~ : State { = "SS TrAdd"; };
        }
        # "Disconnect";
        Dc_Disconn : ASdcDisconn @ {
            _@ < Debug.LogLevel = "Dbg";
            V1 ~ : State { = "SS SResult.Inp"; };
            V2 ~ : State { = "SS TrAdd"; };
        }
        Dc_Conn.Enable ~ Dc_Disconn.Outp;
        # "Form start condition";
        Dc_Disconn.Enable ~ : TrOrVar @ {
            Inp ~ : State @ {
                    _@ < = "SB true";
                    Inp ~ : State { = "SB false"; };
                };
            Inp ~ Dc_Conn.Outp;
        };

    }
}

MyRoot : Elem {
    ServCp : DesSpt {
        # "Service connpoint"
        PinDelta : CpStateInp
        PinIncr : CpStateInp
        PinRes : CpStateOutp
    }
    SpCp : DesSp {
        Debug.LogLevel = "Dbg"
        # "Service point extended CP"
        PinDelta : CpStateInp
        PinIncr : CpStateInp
        PinRes : CpStateOutp
    }
    ServCpm : DesSpt {
        # "Service connpoint"
        PinDelta : CpStateOutp
        PinIncr : CpStateOutp
        PinRes : CpStateInp
    }
    Client : Des {
        # "Client base"
        SpAgent : ADesSpc
        Scp : ServCp
        Incr : State @  {
            _@ <  {
                = "SI 0"
                Debug.LogLevel = "Dbg"
            }
            Inp ~ Scp.PinRes
        }
        Delta : State {
            = "SI 1"
        }
        Scp @  {
            PinDelta ~ Delta
            PinIncr ~ Incr
        }
    }
    # "DES Service point test 1"
    Launcher : DesLauncher {
        Debug.LogLevel = "Dbg"
        Ds1 : Des {
            # "Service system"
            ServPt : DesSpe {
                Debug.LogLevel = "Dbg"
                Int : SpCp
            }
            Ss : Des {
                # "Servicing subsystem. Just adds incrementor value with delta"
                SpAgent : ADesSpc
                Cp : ServCpm
                Cp.PinRes ~ : TrAddVar @  {
                    Inp ~ Cp.PinIncr
                    Inp ~ Cp.PinDelta
                }
            }
            ClientA : Ss
            ServPt.Int ~ ClientA.Cp
            ClientB : Ss
            ServPt.Int ~ ClientB.Cp
        }
        ClientA : Client
        Ds1.ServPt ~ ClientA.Scp
        ClientB : Client
        Ds1.ServPt ~ ClientB.Scp
    }
}

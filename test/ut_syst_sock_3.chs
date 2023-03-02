MyRoot : Elem {
    # "Test 1: compatible sockets"
    # "This is also use-case of IFR rule misunderstanding in the socket extension schema"
    S1 : Syst {
        SockA : Socket {
            Pin1 : ConnPointu {
                Provided = "MTIf1"
                Required = "MTIf2"
            }
        }
        SockB : Socket {
            Pin1 : ConnPointu {
                Provided = "MTIf2"
                Required = "MTIf1"
            }
        }
        Sock1 : Socket {
            PinS1 : SockA
        }
        Sock2 : Socket {
            PinS1 : SockB
        }
        Sock1 ~ Sock2
        S1_1 : Syst {
            Agt : TstAgt
            Cp : ConnPointu {
                Provided = "MTIf2"
                Required = "MTIf1"
            }
        }
        S1_2 : Syst {
            Agt : TstAgt
            E1 : Extd {
                Int : SockB
            }
            Sck1 : SockA
            Cp : ConnPointu {
                Provided = "MTIf2"
                Required = "MTIf1"
            }
            # "ISS_012 Exception when resolving iface in socket extd topology"
            Sck1 ~ E1.Int
            # "This shows misunderstanding in using sockets via extender"
            # "IFR of MTIf2 from S1.Sock2.PinS1.Pin1 doesn't work"
            # "This is because MTIf2 is provided in S1.Sock2.PinS1.Pin1 but it is also provided"
            # "in E1.Int.Pin1 i.e. routed to owner, so IFR stops resolution"
            # "!! Even the connection chain seems legal we need to consider prov/req also"
            Cp ~ Sck1.Pin1
        }
        Sock1.PinS1.Pin1 ~ S1_1.Cp
        Sock2.PinS1 ~ S1_2.E1
    }
}

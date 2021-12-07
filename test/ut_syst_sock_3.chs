MyRoot : Elem {
    # "Test 1: compatible sockets";
    S1 : Syst {
        SockA : Socket {
            Pin1 : ConnPointu {
               Provided = "MTIf1";
               Required = "MTIf2";
            }
        }
        SockB : Socket {
            Pin1 : ConnPointu {
               Provided = "MTIf2";
               Required = "MTIf1";
            }
        }
        Sock1 : Socket {
            PinS1 : SockA;
        }
        Sock2 : Socket {
            PinS1 : SockB;
        }
        Sock1 ~ Sock2;
        S1_1 : Syst {
            Agt : TstAgt;
            Cp : ConnPointu {
               Provided = "MTIf2";
               Required = "MTIf1";
            }
        }
        S1_2 : Syst {
            Agt : TstAgt;
            E1 : Extd {
               Int : SockB;
            }
            Cp : ConnPointu {
               Provided = "MTIf1";
               Required = "MTIf2";
            }
            Cp ~ E1.Int.Pin1;
        }
        Sock1.PinS1.Pin1 ~ S1_1.Cp;
        Sock2.PinS1 ~ S1_2.E1;
    }
}

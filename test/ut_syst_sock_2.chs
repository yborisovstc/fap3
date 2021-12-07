MyRoot : Elem {
    # "Test 2: incompatible sockets";
    S1 : Syst {
        Sock1 : Socket {
            Pin1 : ConnPointu {
               Provided = "MTIf1";
               Required = "Iface2";
            }
        }
        Sock2 : Socket {
            Pin1 : ConnPointu {
               Provided = "Iface2";
               Required = "MTIf1";
            }
            Pin2 : Vert;
        }
        Sock1 ~ Sock2;
        S1 : Syst {
            Agt : TstAgt;
            Cp : ConnPointu {
               Provided = "MTIf1";
               Required = "Iface2";
            }
        }
        Sock2.Pin1 ~ S1.Cp;
    }
}

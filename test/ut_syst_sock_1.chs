MyRoot : Elem {
    S1 : Syst {
        Sock1 : Socket {
            Pin1 : ConnPointu {
               Provided = "Iface1";
               Required = "Iface2";
            }
        }
        Sock2 : Socket {
            Pin1 : ConnPointu {
               Provided = "Iface2";
               Required = "Iface1";
            }
        }
        Sock1 ~ Sock2;
    }
}

MyRoot : Elem {
    # "Test of IFR invalidation: sockets";
    S1 : Syst {
        Sock1 : Socket {
            Pin1 : CpStateInp;
        }
        Sock2 : Socket {
            Pin1 : CpStateOutp;
        }
        Sock1 ~ Sock2;
        State1 : State;
        Sock1.Pin1 ~ State1;
    }
}

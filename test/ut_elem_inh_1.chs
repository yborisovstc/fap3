MyRoot : Elem {
    # "Test of elem inheritance";
    E1 : Elem {
        N1_1 : Node;
        N1_1 < N1_1_1 : Node;
        N1_1 < {
             About : Content;
             About = "N1_1";
        }
        c11 : Content;
        c11 = "Hello";
    }
    E2 : E1 {
        About : Content;
        About = "Heir of E1";
    }
    E3 : E2 {
        About : Content;
        About = "Heir of E2";
    }
}

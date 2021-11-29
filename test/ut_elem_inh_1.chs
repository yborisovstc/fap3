MyRoot : Elem {
    # "Test of elem inheritance";
    E1 : Elem {
        About : Content { = "E1"; }
        N1_1 : Node;
        N1_1 < N1_1_1 : Node;
        N1_1 < {
             About : Content;
             About = "N1_1";
        }
        N1_1 @ {
             About = "N1_1 new";
        }
        c11 : Content;
        c11 = "Hello";
    }
    E1 < {
      E1_1 : Elem;
      E1_1 < About : Content { = "E1_1"; }
    }    
    E2 : E1 {
        About = "Heir of E1";
    }
    E3 : E2 {
        About = "Heir of E2";
    }
}

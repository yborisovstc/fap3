MyRoot : Elem {
    # "Test of elem inheritance";
    E1 : Elem {
        N1_1 : Node;
        N1_1 < N1_1_1 : Node;
        c11 : Content;
        c11 = "Hello";
        N1_1 < {
            N1_1_2 : Node;
        }
       N1_1 < NewNode3 : Node;
    }
    E2 : E1 {
        About : Content;
        About = "Heir of E1";
    }
    E1.N1_1 < NewNode2 : Node;
}

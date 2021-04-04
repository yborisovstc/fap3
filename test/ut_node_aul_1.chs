MyRoot : Elem {
    N1 : Node {
        N1_1 : Node;
        N1_1 < N1_1_1 : Node;
        c11 : Content;
        c11 = "Hello";
    }
    N1 = "Test";
    N1.N1_1 < N1_1_2 : Node;
    N1.N1_1 < ! N1_1_1;
}

MyRoot : Elem {
    n1 : Node {
        n1_1 : Node;
        n1_1 < n1_1_1 : Node;
        c11 : Content;
        c11 = "Hello";
    }
    n1 = "Test";
    n1.n1_1 < n1_1_2 : Node;
    n1.n1_1 < ! n1_1_1;
}

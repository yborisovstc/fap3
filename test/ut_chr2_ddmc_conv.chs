root : Elem {
    v4 ~ v1_27 : v1 @  {
        # "Anonymous namespace"
    }
    Modules : Node {
        + Module1
    }
    + my_mod
    v1 : Vert
    v1 < Cont = "Content 1"
    v2 : Vert
    v3 : Vert
    v2 : v1 {
        e2_1 : Elem
        # "Comment"
    }
    v2 ~ v1_187 : v1 {
        e1_1 : Elem
    }
    v3 ~ e1 @  {
        # "Comment2"
        _@ <  {
            # "Special name namespace"
        }
    }
    v4 ~ v1_429 : v1 <  {
        # "Anonymous target"
    }
    v4 ~ v1_482 : v1 @  {
        # "Anonymous namespace"
    }
    _ <  {
        # "Special name nil"
    }
}
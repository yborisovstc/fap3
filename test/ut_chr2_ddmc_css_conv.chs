root : Elem {
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
    v3 ~ e1  (
        # "Comment2"
        _@ <  {
            # "Special name namespace"
        }
    )
    v4 ~ : v1 <  {
        # "Anonymous target"
    }
    v4 ~ : v1 (
        # "Anonymous namespace"
    )
    v4 ~ : v1 (
        # "Anonymous namespace specific segment"
    )
    _ <  {
        # "Special name nil"
    }
    v5 : Vert (
        # "mut_add with NS segment"
    )
    v6 : Vert <  {
        # "mut_add with Targ segment"
    }
    {
        # "Segment"
    }
}
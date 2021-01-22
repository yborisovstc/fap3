root : Elem
{
    # "Test";
    s1 : Vert
    {
        v1 : Vert @ {
             v2 : Vert;
        }
        v1 ~ : Vert < {
             v3 : Vert;
        };
        v1 ~ : Vert {
             v4 : Vert;
        };
    }
}

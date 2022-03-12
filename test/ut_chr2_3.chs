root : Elem
{
    v3 : Vert @ {
        Value = "Test Value 2";
    }
    # "Test of chrmoo2 dependent mutation chain (DMC): base chromo";
    s1 : Vert
    {
        v1 : Vert;
        v3 : Vert;
        v1 ~ v3;
        v1 < {
            Value = "Test Value";
        }
        v1 ~ v2 : Vert;
        v4 : Vert;
        v5 : Vert;
        v4 ~ v5;
        v4 !~ v5;
    }
    # "Check of how DMC mut is detected";
}

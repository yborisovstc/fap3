root : Elem
{
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
    }
    # "Check of how DMC mut is detected";
}

MyRoot : Elem {
    S1 : Syst {
        V1 : Vertu;
        V2 : Vertu;
        V1 ~ V2;
        L1 : Link;
        L1 ~ _$;
        V3 : Vertu;
        V4 : Vertu;
        V3 ~ V4;
        V3 !~ V4;
    }
}

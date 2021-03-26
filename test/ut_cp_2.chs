MyRoot : Elem {
    S1 : Syst {
        Cp1 : ConnPointu { Provided = "Ifc1"; Required = "Ifc2"; }
        Cp2 : ConnPointu { Provided = "Ifc2"; Required = "Ifc1"; }
        Cp1 ~ Cp2;
    }
}

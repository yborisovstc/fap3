AdpComps : Elem {
    # "DES adapter module";
    About : Content { = "Agents DES adaptor module"; }
    NodeAdp : Syst
    {
        # "Add content AgentUri to set managed agent URI";
        About : Content { = "MNode DES adaptor"; }
        AdpAgent : AMnodeAdp;
        InpMagUri : CpStateInp;
        CompsCount : CpStateOutp;
        CompNames : CpStateOutp;
        Owner : CpStateOutp;
    }
    ElemAdp : Syst
    {
        About : Content { = "MElem DES adaptor"; }
        AdpAgent : AMelemAdp;
        InpMut : CpStateInp;
    }
}

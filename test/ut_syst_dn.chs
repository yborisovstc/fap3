MyRoot : Elem {
    # "Test on chromo runtime dependency node"
    S1 : Syst {
        V1 : Vert
        # "Q-dep segent_target"
        V1 ~ VS1 : Vert <  {
            V_E : Elem
        }
        # "Q-dep mutation_create"
        V1 ~ : Vert {
            # "Anonymous vertex"
        }
        V3 : Vert {
            V3_N1 : Node
        }
        # "Q-dep segent_target"
        V1 ~ V3 <  {
            V3_E : Elem
        }
        # "Q-dep segment_namespace"
        V4 : Vert
        V4 ~ V3  (
            V3_E < Cont = "Content"
        )
        # "Q-dep segment_namespace"
        V4 ~ VS2 : Vert (
            E5 : Elem
        )
        V3I : V3
    }
}

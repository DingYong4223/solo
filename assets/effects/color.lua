{
    vertex = {
        uniformBuffers = {
            matrices = {
                wvp = "mat4"
            }
        },

        inputs = {
            sl_Position = "vec4"
        },

        code = [[
            void main()
            {
                gl_Position = #matrices:wvp# * sl_Position;
                SL_FIX_Y#gl_Position#;
            }
        ]]
    },

    fragment = {
        uniformBuffers = {
            variables = {
                color = "vec4"
            }
        },

        outputs = {
            fragColor = { type = "vec4", target = 0 }
        },

        code = [[
            void main()
            {
                fragColor = #variables:color#;
            }
        ]]
    }
}

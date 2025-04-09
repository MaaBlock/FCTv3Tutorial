ShaderOut main(ShaderIn in) {
    out.target[0] = in.color;
    out.depth = 1.0f;
    return out;
}
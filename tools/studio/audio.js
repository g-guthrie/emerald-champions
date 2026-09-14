class EmeraldAudio extends AudioWorkletProcessor {
  constructor() {
    super();
    this.samples = new Float32Array(16384);
    this.read = 0; this.write = 0; this.phase = 0; this.started = false;
    this.port.onmessage = ({data}) => {
      if (data.reset) { this.read = this.write; this.started = false; this.phase = 0; return; }
      const pcm = new Int16Array(data);
      for (let i = 0; i < pcm.length; i++) this.samples[this.write++ & 16383] = pcm[i] / 32768;
      if (this.write - this.read > 4096) { this.read = this.write - 2048; this.phase = 0; }
    };
  }
  process(inputs, outputs) {
    const channels = outputs[0];
    if (!this.started && this.write - this.read >= 2048) this.started = true;
    for (let i = 0; i < channels[0].length; i++) {
      if (!this.started || this.write - this.read < 4) {
        channels[0][i] = channels[1][i] = 0; this.started = false; continue;
      }
      for (let ch = 0; ch < 2; ch++) {
        const a = this.samples[(this.read + ch) & 16383], b = this.samples[(this.read + ch + 2) & 16383];
        channels[ch][i] = a + (b - a) * this.phase;
      }
      this.phase += 32768 / sampleRate;
      const advance = Math.floor(this.phase);
      this.read += advance * 2; this.phase -= advance;
    }
    return true;
  }
}
registerProcessor("emerald-audio", EmeraldAudio);

RSpec.describe Rure do
  it 'has a version number' do
    expect(Rure::VERSION).not_to be nil
  end

  describe Rure::Regex do
    describe '#match?' do
      subject { Rure::Regex.new('ab').match?(haystack) }

      context 'when match exists' do
        let(:haystack) { 'xxab' }
        it { is_expected.to be true }
      end

      context 'when a match does not exist' do
        let(:haystack) { 'xx' }
        it { is_expected.to be false }
      end
    end

    describe '#find' do
      subject { Rure::Regex.new('ab').find(haystack) }

      context 'when match exists' do
        let(:haystack) { 'xxab' }
        it { is_expected.to eq(Rure::Match.new(2,4)) }
      end

      context 'when a match does not exist' do
        let(:haystack) { 'xx' }
        it { is_expected.to be_nil }
      end
    end

    describe '#each_match' do
      let(:haystack) { 'xabcab' }

      context 'when no block is given' do
        subject { Rure::Regex.new('ab').each_match(haystack) }
        it { is_expected.to be_a Enumerator }
      end

      context 'when block is given' do
        it 'should iterate over the non-overlapping matches in order' do
          matches = []
          Rure::Regex.new('ab').each_match(haystack) do |match|
            matches << match
          end
          expect(matches).to eq [Rure::Match.new(1,3), Rure::Match.new(4,6)]
        end
      end
    end
  end
end

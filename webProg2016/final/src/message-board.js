const { Component, PropTypes } = React;

const emojiOut = emotionIn => {
  if (emotionIn === 'sad') return '1f61e';
  if (emotionIn === 'happy') return '1f603';
  if (emotionIn === 'angry') return '1f621';
  if (emotionIn === 'surprised') return '1f62f';
  return '1f6ab';
};


const MessageBlock = ({
  textIn,
  visitorIn,
  timeIn,
  emotionIn
}) => (
  <div className="MessageBlock">
    <div className="MessageBody">
      "{textIn}", said {visitorIn}.
      <br /><br />
      {timeIn}
    </div>
    <img src={`./dist/png_512/${emojiOut(emotionIn)}.png`} />
  </div>
);

class MessageBoard extends Component {
  constructor(props) {
    super(props);
    this.state = {
      nowTime: new Date(),
      blockArr: [],
    };
  }
  componentDidMount() {
    this.startTick = setInterval(() => {
      this.setState({ nowTime: new Date() });
    }, 1000);
  }
  componentWillUnmount() { clearInterval(this.startTick); }
  render() {
    return (
      <div className="MessageBoard">
        <div className="blockArr">{this.state.blockArr}</div>
        <div className="nowTime">{this.state.nowTime.toString()}</div>
      </div>
    );
  }
}

ReactDOM.render(<MessageBlock
  textIn="Just a dream~~~"
  visitorIn="admin"
  timeIn={(new Date()).toString()}
  emotionIn="happy"
/>, document.getElementById('root'));
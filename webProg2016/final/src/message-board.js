const { Component, PropTypes } = React;

const emojiOut = emotionIn => {
  if (emotionIn === 'sad') return '1f61e';
  if (emotionIn === 'happy') return '1f603';
  if (emotionIn === 'angry') return '1f621';
  if (emotionIn === 'surprised') return '1f62f';
  return '1f6ab';
};

const isOdd = orderIn => {
  if (orderIn % 2 === 1) return 'oddColor';
  return 'evenColor';
}

const MessageBlock = ({
  orderIn,
  textIn,
  visitorIn,
  timeIn,
  emotionIn
}) => (
  <div className={`MessageBlock ${isOdd(orderIn)}`}>
    <div className="MessageBody">
      "{textIn}", said {visitorIn}.
      <br /><br />
      {timeIn}
    </div>
    <img src={`./dist/png_512/${emojiOut(emotionIn)}.png`} />
  </div>
);
MessageBlock.defaultProps = {
  visitorIn: 'admin',
  timeIn: (new Date()).toString(),
  emotionIn: 'no preference',
};

class MessageBoard extends Component {
  constructor(props) {
    super(props);
    this.state = {
      nowTime: new Date(),
      blockArr: [
        <MessageBlock
          orderIn={0}
          textIn="Go Ahead!"
          emotionIn="happy"
        />,
        <MessageBlock
          orderIn={1}
          textIn="Let's Chat!"
          emotionIn="happy"
        />
      ],
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
        <textarea placeholder="Text here to leave a message..." onKeyUp={e => {
          this.visitorName = e.target.value;
        }} />
        <input placeholder="Nickname, please." onKeyUp={e => {
          this.visitorText = e.target.value;
        }} />
        <img src="./dist/png_512/1f603.png" />
        <img src="./dist/png_512/1f62f.png" />
        <img src="./dist/png_512/1f61e.png" />
        <img src="./dist/png_512/1f621.png" />
        <img src="./dist/png_512/1f6ab.png" />
        <div className="nowTime">{this.state.nowTime.toString()}</div>
      </div>
    );
  }
}

/* ReactDOM.render(<MessageBlock
  textIn="Just a dream~~~"
  visitorIn="admin"
  timeIn={(new Date()).toString()}
  emotionIn="happy"
/>, document.getElementById('root')); */

ReactDOM.render(<MessageBoard />, document.getElementById('root'));
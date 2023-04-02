const homedir = require('os').homedir();
const { Configuration, OpenAIApi } = require("openai");
const config = require(PRODUCTION ? homedir + "/config.json" : "./config.json");

const configuration = new Configuration({
  apiKey: config.CHATGPT_TOKEN,
});
const openai = new OpenAIApi(configuration);

const chatGPT = async (text) => {
	logT("token", config.CHATGPT_TOKEN)
	const completion = await openai.createChatCompletion({
		model: "gpt-3.5-turbo",
		messages: [{"role": "user", "content": text}],
	});
	const answer = completion.data.choices[0]?.message?.content;
	logT('chatgpt', 'question:', text, 'answer:', answer);
	return answer;
}

module.exports = {
    chatGPT
};
